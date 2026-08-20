#pragma once
#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>
#include <string>

namespace fs = std::filesystem;

using json = nlohmann::json;

namespace gerneral{
inline std::string configPath = std::string(getenv("HOME")) + "/.config/soundboard/config.json";
enum Tab{
    Soundboard,
    Mic,
    Settings,
    None
};
inline Tab currentTab = Soundboard;

enum btn_Group{
    Tab_Group,
    Soundboard_Group
};

inline btn_Group current_Group;

inline float AddSoundButtonY = 60;

inline bool justAdded = false;
inline float scrollOffset = 0.0f;
inline int SoundID =0;
}
