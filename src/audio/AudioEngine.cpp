#include "AudioEngine.h"
#include "../platform/Platform.h"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <memory>
#include <mutex>
#include <set>
#include <thread>
#include <unordered_map>

#define MA_NO_ENCODING
#define MA_NO_GENERATION
#define MA_NO_FLAC
#define MA_NO_ENGINE
#define MA_NO_NODE_GRAPH
#define MA_NO_RESOURCE_MANAGER
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

namespace Audio {
namespace {

constexpr ma_uint32 kRate = 48000;
constexpr ma_uint32 kChunk = 1024;
constexpr size_t kMaxVoices = 32;
constexpr size_t kRingSize = 48000;
constexpr size_t kPrime = 1920;
constexpr size_t kMaxLag = 14400;
constexpr size_t kKeep = 3840;

#ifdef _WIN32
const char* kDefaultName = "Windows-Standard";
#else
const char* kDefaultName = "System-Standard";
#endif

using Pcm = std::shared_ptr<const std::vector<int16_t>>;

struct Voice {
    Pcm data;
    size_t pos = 0;
    bool local = false;
};

struct Ring {
    std::vector<float> buf;
    std::atomic<size_t> w{0};
    std::atomic<size_t> r{0};
    bool primed = false;

    void Reset(){
        buf.assign(kRingSize, 0.0f);
        w = 0;
        r = 0;
        primed = false;
    }

    void Push(const float* d, size_t n){
        size_t wp = w.load(std::memory_order_relaxed);
        size_t rp = r.load(std::memory_order_acquire);
        size_t room = buf.size() - (wp - rp);
        if(n > room) n = room;
        for(size_t i = 0; i < n; i++) buf[(wp + i) % buf.size()] = d[i];
        w.store(wp + n, std::memory_order_release);
    }

    size_t Pop(float* d, size_t n){
        size_t rp = r.load(std::memory_order_relaxed);
        size_t wp = w.load(std::memory_order_acquire);
        size_t avail = wp - rp;
        if(avail > kMaxLag){
            rp = wp - kKeep;
            avail = kKeep;
        }
        if(!primed){
            if(avail < kPrime){
                r.store(rp, std::memory_order_release);
                return 0;
            }
            primed = true;
        }
        size_t k = std::min(n, avail);
        for(size_t i = 0; i < k; i++) d[i] = buf[(rp + i) % buf.size()];
        r.store(rp + k, std::memory_order_release);
        if(k < n) primed = false;
        return k;
    }
};

ma_context ctx;
bool ctxOk = false;
ma_device primary;
ma_device secondary;
bool primaryOk = false;
bool secondaryOk = false;
bool hasVirtual = false;

std::vector<std::string> inNames, outNames, virtNames;
std::vector<ma_device_id> inIds, outIds;
std::string status = "Audio nicht gestartet";
std::string sinkModule;
std::string virtMic;
std::string defaultInName;

ma_log logger;
bool logOk = false;
std::mutex logMutex;
std::string logPath;

void LogCb(void*, ma_uint32 level, const char* msg){
    if(level > MA_LOG_LEVEL_WARNING) return;
    std::string line = std::string(level == MA_LOG_LEVEL_ERROR ? "[Fehler] " : "[Warnung] ") + msg;
    while(!line.empty() && (line.back() == '\n' || line.back() == '\r')) line.pop_back();
    std::lock_guard<std::mutex> lk(logMutex);
    if(logPath.empty()) return;
    std::ofstream f(std::filesystem::u8path(logPath), std::ios::app);
    f << line << "\n";
}

std::mutex vm;
std::vector<Voice> voices;

std::mutex cm;
std::unordered_map<std::string, Pcm> cache;
std::set<std::string> loading;
std::atomic<int> pending{0};

std::atomic<int> mode{2};
std::atomic<bool> inputTest{false};
std::atomic<int> retrigger{0};
std::atomic<unsigned> epoch{0};
std::atomic<float> level{0.0f};
Ring ring;

inline float Clip(float x){ return x > 1.0f ? 1.0f : (x < -1.0f ? -1.0f : x); }

std::string Lower(std::string s){
    for(auto& c : s) c = (char)std::tolower((unsigned char)c);
    return s;
}

void Enumerate(){
    inNames = {kDefaultName};
    outNames = {kDefaultName};
    inIds.clear();
    outIds.clear();
    defaultInName.clear();
    if(ctxOk){
        ma_device_info* pp = nullptr;
        ma_device_info* cp = nullptr;
        ma_uint32 pc = 0, cc = 0;
        if(ma_context_get_devices(&ctx, &pp, &pc, &cp, &cc) == MA_SUCCESS){
            for(ma_uint32 i = 0; i < cc; i++){
                inNames.push_back(cp[i].name);
                if(cp[i].isDefault) defaultInName = cp[i].name;
                inIds.push_back(cp[i].id);
            }
            for(ma_uint32 i = 0; i < pc; i++){
                outNames.push_back(pp[i].name);
                outIds.push_back(pp[i].id);
            }
        }
    }
    virtNames = {"Automatisch", "Keiner"};
    for(size_t i = 1; i < outNames.size(); i++) virtNames.push_back(outNames[i]);
}

const ma_device_id* Find(const std::vector<std::string>& names, const std::vector<ma_device_id>& ids, const std::string& name){
    if(name.empty()) return nullptr;
    for(size_t i = 1; i < names.size(); i++)
        if(names[i] == name) return &ids[i - 1];
    return nullptr;
}

bool IsVirtualName(const std::string& name){
    std::string n = Lower(name);
    for(const char* k : {"cable", "voicemeeter", "azavdiap", "monitor of", "virtual"})
        if(n.find(k) != std::string::npos) return true;
    return false;
}

int AutoVirtual(){
    static const char* keys[] = {"azavdiap", "cable input", "voicemeeter input", "voicemeeter aux input"};
    for(auto k : keys)
        for(size_t i = 1; i < outNames.size(); i++)
            if(Lower(outNames[i]).find(k) != std::string::npos) return (int)i - 1;
    return -1;
}

void Mix(Voice& v, float* dst, size_t n){
    size_t total = v.data->size() / 2;
    size_t k = std::min(n, total - v.pos);
    const int16_t* s = v.data->data() + v.pos * 2;
    for(size_t i = 0; i < k * 2; i++) dst[i] += s[i] * (1.0f / 32768.0f);
    v.pos += k;
}

void PrimaryCb(ma_device*, void* pOut, const void* pIn, ma_uint32 frames){
    float* out = (float*)pOut;
    const float* mic = (const float*)pIn;
    const int m = mode.load() | (inputTest.load() ? 1 : 0);
    const bool virt = hasVirtual;
    float peak = 0.0f;
    float snd[kChunk * 2], loc[kChunk * 2], mon[kChunk * 2];

    for(ma_uint32 done = 0; done < frames;){
        ma_uint32 n = std::min(kChunk, frames - done);
        std::memset(snd, 0, n * 2 * sizeof(float));
        std::memset(loc, 0, n * 2 * sizeof(float));
        {
            std::lock_guard<std::mutex> lk(vm);
            for(auto& v : voices) Mix(v, v.local ? loc : snd, n);
            voices.erase(std::remove_if(voices.begin(), voices.end(),
                [](const Voice& v){ return v.pos >= v.data->size() / 2; }), voices.end());
        }
        for(ma_uint32 i = 0; i < n; i++){
            float x = mic ? mic[done + i] : 0.0f;
            peak = std::max(peak, std::fabs(x));
            for(int ch = 0; ch < 2; ch++){
                float s = snd[i * 2 + ch];
                float o = ((m & 1) ? x : 0.0f) + ((m & 2) ? s : 0.0f) + loc[i * 2 + ch];
                if(virt){
                    out[(done + i) * 2 + ch] = Clip(x + s);
                    mon[i * 2 + ch] = Clip(o);
                }
                else out[(done + i) * 2 + ch] = Clip(o);
            }
        }
        if(virt) ring.Push(mon, n * 2);
        done += n;
    }
    level = std::max(peak, level.load() * 0.85f);
}

void SecondaryCb(ma_device*, void* pOut, const void*, ma_uint32 frames){
    float* out = (float*)pOut;
    size_t want = (size_t)frames * 2;
    size_t got = ring.Pop(out, want);
    if(got < want) std::memset(out + got, 0, (want - got) * sizeof(float));
}

void StopDevices(){
    if(primaryOk){ ma_device_uninit(&primary); primaryOk = false; }
    if(secondaryOk){ ma_device_uninit(&secondary); secondaryOk = false; }
    hasVirtual = false;
}

std::string MicNameFor(const std::string& playback){
#ifdef _WIN32
    std::string name = playback;
    size_t p = name.rfind("Input");
    if(p != std::string::npos) name.replace(p, 5, "Output");
    return name;
#else
    return "Monitor of " + playback;
#endif
}

Pcm Decode(const std::string& path){
    ma_decoder_config dc = ma_decoder_config_init(ma_format_s16, 2, kRate);
    ma_decoder dec;
#ifdef _WIN32
    int wn = MultiByteToWideChar(CP_UTF8, 0, path.c_str(), -1, nullptr, 0);
    if(wn <= 1) return nullptr;
    std::wstring w(wn - 1, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, path.c_str(), -1, &w[0], wn);
    ma_result r = ma_decoder_init_file_w(w.c_str(), &dc, &dec);
#else
    ma_result r = ma_decoder_init_file(path.c_str(), &dc, &dec);
#endif
    if(r != MA_SUCCESS) return nullptr;

    auto out = std::make_shared<std::vector<int16_t>>();
    ma_uint64 total = 0;
    if(ma_decoder_get_length_in_pcm_frames(&dec, &total) == MA_SUCCESS && total > 0 && total < (1ull << 30))
        out->reserve((size_t)total * 2);
    std::vector<int16_t> chunk(4096 * 2);
    while(true){
        ma_uint64 got = 0;
        ma_decoder_read_pcm_frames(&dec, chunk.data(), 4096, &got);
        if(got == 0) break;
        out->insert(out->end(), chunk.begin(), chunk.begin() + (size_t)got * 2);
    }
    ma_decoder_uninit(&dec);
    if(out->empty()) return nullptr;
    out->shrink_to_fit();
    return out;
}

Pcm Cached(const std::string& path){
    std::lock_guard<std::mutex> lk(cm);
    auto it = cache.find(path);
    return it == cache.end() ? nullptr : it->second;
}

void AddVoice(Pcm d, bool local){
    std::lock_guard<std::mutex> lk(vm);
    if(voices.size() >= kMaxVoices) return;
    Voice v;
    v.data = std::move(d);
    v.local = local;
    voices.push_back(std::move(v));
}

Pcm ToneData(){
    static Pcm tone = []{
        auto v = std::make_shared<std::vector<int16_t>>();
        const size_t frames = (size_t)(kRate * 0.6);
        for(size_t i = 0; i < frames; i++){
            float t = (float)i / kRate;
            float f = t < 0.3f ? 440.0f : 660.0f;
            float env = std::min(1.0f, std::min(i / 480.0f, (frames - i) / 4800.0f));
            int16_t s = (int16_t)(std::sin(6.2831853f * f * t) * 0.3f * env * 32767.0f);
            v->push_back(s);
            v->push_back(s);
        }
        return Pcm(v);
    }();
    return tone;
}

}

bool Init(){
#ifndef _WIN32
    std::string id = Platform::RunCommand("pactl load-module module-null-sink sink_name=AZAVDIAP sink_properties=device.description=AZAVDIAP 2>/dev/null");
    if(!id.empty() && id.find_first_not_of("0123456789") == std::string::npos) sinkModule = id;
#endif
    logOk = ma_log_init(nullptr, &logger) == MA_SUCCESS;
    ma_context_config cc = ma_context_config_init();
    if(logOk){
        ma_log_register_callback(&logger, ma_log_callback_init(LogCb, nullptr));
        cc.pLog = &logger;
    }
    ctxOk = ma_context_init(nullptr, 0, &cc, &ctx) == MA_SUCCESS;
    ring.Reset();
    Enumerate();
    if(!ctxOk) status = "Audio-System konnte nicht gestartet werden";
    return ctxOk;
}

void Shutdown(){
    StopDevices();
    while(pending > 0) std::this_thread::sleep_for(std::chrono::milliseconds(10));
    if(ctxOk){ ma_context_uninit(&ctx); ctxOk = false; }
    if(logOk){ ma_log_uninit(&logger); logOk = false; }
#ifndef _WIN32
    if(!sinkModule.empty()) Platform::RunCommand("pactl unload-module " + sinkModule + " 2>/dev/null");
#endif
}

void RefreshDevices(){ Enumerate(); }

const std::vector<std::string>& InputNames(){ return inNames; }
const std::vector<std::string>& OutputNames(){ return outNames; }
const std::vector<std::string>& VirtualNames(){ return virtNames; }
const std::string& Status(){ return status; }
bool HasVirtual(){ return hasVirtual; }
std::string Backend(){ return ctxOk ? ma_get_backend_name(ctx.backend) : "keins"; }

void SetLogFile(const std::string& path){
    std::lock_guard<std::mutex> lk(logMutex);
    logPath = path;
    std::ofstream f(std::filesystem::u8path(path), std::ios::trunc);
}
const std::string& VirtualMic(){ return virtMic; }

ma_result OpenPrimary(const ma_device_id* inId, const ma_device_id* playId, bool& micOk, ma_performance_profile profile){
    ma_device_config c = ma_device_config_init(ma_device_type_duplex);
    c.sampleRate = kRate;
    c.capture.pDeviceID = inId;
    c.capture.format = ma_format_f32;
    c.capture.channels = 1;
    c.playback.pDeviceID = playId;
    c.playback.format = ma_format_f32;
    c.playback.channels = 2;
    c.dataCallback = PrimaryCb;
    c.performanceProfile = profile;

    micOk = true;
    ma_result r = ma_device_init(&ctx, &c, &primary);
    if(r == MA_SUCCESS){
        r = ma_device_start(&primary);
        if(r != MA_SUCCESS) ma_device_uninit(&primary);
    }
    if(r == MA_SUCCESS) return r;

    micOk = false;
    c.deviceType = ma_device_type_playback;
    r = ma_device_init(&ctx, &c, &primary);
    if(r == MA_SUCCESS){
        r = ma_device_start(&primary);
        if(r != MA_SUCCESS) ma_device_uninit(&primary);
    }
    return r;
}

ma_result OpenWithRetry(const ma_device_id* inId, const ma_device_id* playId, bool& micOk){
    ma_result r = OpenPrimary(inId, playId, micOk, ma_performance_profile_low_latency);
    for(int attempt = 0; attempt < 3 && r == MA_BUSY; attempt++){
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
        r = OpenPrimary(inId, playId, micOk, ma_performance_profile_low_latency);
    }
    if(r != MA_SUCCESS) r = OpenPrimary(inId, playId, micOk, ma_performance_profile_conservative);
    return r;
}

bool Start(const std::string& in, const std::string& out, const std::string& virt){
    StopDevices();
    if(!ctxOk) return false;

    const ma_device_id* inId = Find(inNames, inIds, in);
    const ma_device_id* outId = Find(outNames, outIds, out);
    const ma_device_id* virtId = nullptr;
    std::string virtName;
    if(virt != kNone){
        int idx = -1;
        if(virt.empty()) idx = AutoVirtual();
        else for(size_t i = 1; i < outNames.size(); i++) if(outNames[i] == virt) idx = (int)i - 1;
        if(idx >= 0 && outNames[idx + 1] != out){
            virtId = &outIds[idx];
            virtName = outNames[idx + 1];
        }
    }
    hasVirtual = virtId != nullptr;
    virtMic = hasVirtual ? MicNameFor(virtName) : "";
    ring.Reset();

    std::string inNote;
    std::string usedIn = in.empty() ? defaultInName : in;
    if(hasVirtual && in.empty() && IsVirtualName(defaultInName)){
        for(size_t i = 1; i < inNames.size(); i++){
            if(IsVirtualName(inNames[i])) continue;
            inId = &inIds[i - 1];
            usedIn = inNames[i];
            inNote = "Standard-Eingabe war das virtuelle Mikrofon, nutze stattdessen '" + usedIn + "'";
            break;
        }
    }

    std::string note;
    bool micOk = true;
    ma_result r = OpenWithRetry(inId, hasVirtual ? virtId : outId, micOk);
    if(r != MA_SUCCESS && hasVirtual){
        note = "Fehler: Virtuelles Mikrofon '" + virtName + "' konnte nicht geöffnet werden (" + ma_result_description(r) + ") [" + Backend() + "]";
        hasVirtual = false;
        virtMic.clear();
        r = OpenWithRetry(inId, outId, micOk);
    }
    if(r != MA_SUCCESS){
        hasVirtual = false;
        virtMic.clear();
        status = std::string("Fehler: Ausgabegerät konnte nicht geöffnet werden (") + ma_result_description(r) + ") [" + Backend() + "]";
        return false;
    }
    primaryOk = true;

    if(hasVirtual){
        ma_device_config s = ma_device_config_init(ma_device_type_playback);
        s.sampleRate = kRate;
        s.playback.pDeviceID = outId;
        s.playback.format = ma_format_f32;
        s.playback.channels = 2;
        s.dataCallback = SecondaryCb;
        if(ma_device_init(&ctx, &s, &secondary) == MA_SUCCESS){
            if(ma_device_start(&secondary) == MA_SUCCESS) secondaryOk = true;
            else ma_device_uninit(&secondary);
        }
    }

    if(!note.empty()) status = note + " - nur lokale Ausgabe";
    else if(hasVirtual) status = "Spiel-Ausgang: " + virtName;
    else status = "Kein virtuelles Mikrofon gefunden - nur lokale Ausgabe";
    if(!micOk) status += " | Mikrofon nicht verfügbar";
    else if(hasVirtual && !secondaryOk) status += " | Abhörgerät nicht verfügbar";
    if(!inNote.empty()) status += " | " + inNote;
    if(hasVirtual && micOk && usedIn == virtMic) status += " | Warnung: Eingabe ist das virtuelle Mikrofon selbst, bitte echtes Mikrofon wählen";
    return true;
}

void SetMonitorMode(int m){ mode = m; }
void SetInputTest(bool on){ inputTest = on; }
bool InputTest(){ return inputTest; }
float InputLevel(){ return level; }
void PlayTestTone(){ AddVoice(ToneData(), true); }

void Play(const std::string& path){
    if(path.empty()) return;
    Pcm d;
    {
        std::lock_guard<std::mutex> lk(cm);
        auto it = cache.find(path);
        if(it != cache.end()) d = it->second;
        else if(!loading.insert(path).second) return;
    }
    if(d){
        AddVoice(d, false);
        return;
    }
    pending++;
    const unsigned startEpoch = epoch;
    std::thread([path, startEpoch]{
        Pcm data = Decode(path);
        {
            std::lock_guard<std::mutex> lk(cm);
            if(data) cache[path] = data;
            loading.erase(path);
        }
        if(data && startEpoch == epoch) AddVoice(data, false);
        pending--;
    }).detach();
}

bool IsPlaying(const std::string& path){
    Pcm d = Cached(path);
    if(!d) return false;
    std::lock_guard<std::mutex> lk(vm);
    for(auto& v : voices) if(v.data == d) return true;
    return false;
}

void Stop(const std::string& path){
    Pcm d = Cached(path);
    if(!d) return;
    std::lock_guard<std::mutex> lk(vm);
    voices.erase(std::remove_if(voices.begin(), voices.end(), [&](const Voice& v){ return v.data == d; }), voices.end());
}

void StopAll(){
    epoch++;
    std::lock_guard<std::mutex> lk(vm);
    voices.clear();
}

void SetRetriggerMode(int m){ retrigger = m; }

void Trigger(const std::string& path){
    int m = retrigger;
    if(m == 2 && IsPlaying(path)){
        Stop(path);
        return;
    }
    if(m == 1) Stop(path);
    Play(path);
}

}
