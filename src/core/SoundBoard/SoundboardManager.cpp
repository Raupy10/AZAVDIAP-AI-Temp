#include "SoundboardManager.h"
#include "../Hotkeys.h"
#include "../LoadJson.h"
#include <algorithm>

namespace {
constexpr float kRowHeight = 60.0f;
constexpr float kTop = 60.0f;
constexpr float kViewTop = 50.0f;
constexpr float kViewHeight = 400.0f;

Rectangle AddRect(){
    return {60, kTop + Sounds.size() * kRowHeight - gerneral::scrollOffset, 730, 50};
}

float MaxScroll(){
    float content = kTop + (Sounds.size() + 1) * kRowHeight;
    return std::max(0.0f, content - (kViewTop + kViewHeight));
}
}

void SyncHotkeys(){
    std::vector<Hotkeys::Bind> binds;
    for(auto& s : Sounds)
        if(!s.GetKey().empty() && !s.GetPath().empty()) binds.push_back({s.GetKey(), s.GetPath()});
    if(!gerneral::stopAllKey.empty()) binds.push_back({gerneral::stopAllKey, "", true});
    Hotkeys::SetBinds(binds);
}

void SaveSounds(){
    json arr = json::array();
    for(auto& s : Sounds) arr.push_back({{"id", s.GetID()}, {"key", s.GetKey()}, {"path", s.GetPath()}});
    LoadJson::cfg["sounds"] = arr;
    LoadJson::SaveGeneral();
    SyncHotkeys();
}

void LoadSounds(){
    Sounds.clear();
    for(auto& s : LoadJson::cfg["sounds"])
        Sounds.emplace_back(s.value("id", 0), s.value("key", ""), s.value("path", ""));
    SyncHotkeys();
}

void AddSound(const std::string& key, const std::string& path){
    Sounds.emplace_back(++gerneral::SoundID, key, path);
    SaveSounds();
}

int ScanSoundsFolder(){
    if(gerneral::soundsFolder.empty()) return -1;
    std::error_code ec;
    fs::path root = fs::u8path(gerneral::soundsFolder);
    if(!fs::is_directory(root, ec)) return -1;

    int added = 0;
    auto opts = fs::directory_options::skip_permission_denied;
    for(fs::recursive_directory_iterator it(root, opts, ec), end; !ec && it != end; it.increment(ec)){
        std::error_code e2;
        if(!it->is_regular_file(e2)) continue;
        std::string ext = it->path().extension().u8string();
        std::transform(ext.begin(), ext.end(), ext.begin(), [](unsigned char c){ return (char)std::tolower(c); });
        if(ext != ".mp3") continue;
        std::string p = it->path().u8string();
        bool known = std::any_of(Sounds.begin(), Sounds.end(), [&](const SoundboardEntry& s){ return s.GetPath() == p; });
        if(known) continue;
        Sounds.emplace_back(++gerneral::SoundID, "", p);
        added++;
    }
    if(added > 0) SaveSounds();
    return added;
}

void UpdateSoundboard(){
    if(GetMouseWheelMove() != 0) gerneral::scrollOffset -= GetMouseWheelMove() * 30.0f;
    gerneral::scrollOffset = std::clamp(gerneral::scrollOffset, 0.0f, MaxScroll());

    bool changed = false;
    int removeIndex = -1;
    for(size_t i = 0; i < Sounds.size(); i++){
        float y = kTop + i * kRowHeight - gerneral::scrollOffset;
        auto r = Sounds[i].Update(y);
        if(r == SoundboardEntry::Result::Changed) changed = true;
        if(r == SoundboardEntry::Result::Remove) removeIndex = (int)i;
    }

    if(removeIndex >= 0){
        if(SoundboardEntry::activeKeyId == Sounds[removeIndex].GetID()) SoundboardEntry::activeKeyId = -1;
        Sounds.erase(Sounds.begin() + removeIndex);
        changed = true;
    }
    else if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && GetMousePosition().y >= kViewTop && CheckCollisionPointRec(GetMousePosition(), AddRect())){
        AddSound("", "");
    }

    if(changed) SaveSounds();
    gerneral::scrollOffset = std::clamp(gerneral::scrollOffset, 0.0f, MaxScroll());
    Hotkeys::SetSuspended(SoundboardEntry::activeKeyId != -1);
}

void RenderSoundboard(){
    for(size_t i = 0; i < Sounds.size(); i++)
        Sounds[i].Draw(kTop + i * kRowHeight - gerneral::scrollOffset);

    Rectangle add = AddRect();
    DrawRectangleRoundedLines(add, 0.5, 1, DARKGRAY);
    DrawTextureEx(Add_Icon, Vector2{add.x + 340, add.y + 12}, 0.0f, 1.0f, WHITE);

    float max = MaxScroll();
    if(max > 0){
        float barH = kViewHeight * kViewHeight / (kViewHeight + max);
        float barY = kViewTop + (kViewHeight - barH) * (gerneral::scrollOffset / max);
        DrawRectangleRounded({794, barY, 4, barH}, 1.0f, 4, GRAY);
    }
}
