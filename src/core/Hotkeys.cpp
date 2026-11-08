#include "Hotkeys.h"
#include "../audio/AudioEngine.h"
#include "../platform/Platform.h"
#include <atomic>
#include <chrono>
#include <mutex>
#include <thread>

namespace Hotkeys {
namespace {

struct Active {
    Platform::Combo combo;
    std::string path;
    bool stopAll = false;
    bool wasDown = false;
};

std::mutex mtx;
std::vector<Active> active;
std::atomic<bool> enabled{true};
std::atomic<bool> suspended{false};
std::atomic<bool> running{false};
std::thread worker;

void Loop(){
    bool ok = Platform::KeysInit();
    struct Fire { std::string path; bool stopAll; };
    std::vector<Fire> fire;
    while(running){
        std::this_thread::sleep_for(std::chrono::milliseconds(8));
        if(!ok) continue;
        fire.clear();
        {
            std::lock_guard<std::mutex> lk(mtx);
            if(active.empty()) continue;
            Platform::SampleKeys();
            for(auto& a : active){
                bool down = Platform::ComboDown(a.combo);
                if(down && !a.wasDown && enabled && !suspended) fire.push_back({a.path, a.stopAll});
                a.wasDown = down;
            }
        }
        for(auto& f : fire){
            if(f.stopAll) Audio::StopAll();
            else Audio::Trigger(f.path);
        }
    }
    Platform::KeysClose();
}

}

void Start(){
    if(running) return;
    running = true;
    worker = std::thread(Loop);
}

void Stop(){
    running = false;
    if(worker.joinable()) worker.join();
}

void SetBinds(const std::vector<Bind>& binds){
    std::vector<Active> next;
    for(auto& b : binds){
        Active a;
        if((b.path.empty() && !b.stopAll) || !Platform::ParseCombo(b.combo, a.combo)) continue;
        a.path = b.path;
        a.stopAll = b.stopAll;
        next.push_back(a);
    }
    std::lock_guard<std::mutex> lk(mtx);
    active = std::move(next);
}

void SetEnabled(bool on){ enabled = on; }
void SetSuspended(bool on){ suspended = on; }

}
