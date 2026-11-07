#pragma once
#include <nlohmann/json.hpp>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>

namespace fs = std::filesystem;

using json = nlohmann::json;

namespace gerneral{
inline std::string configDir(){
#ifdef _WIN32
    const char* base = std::getenv("APPDATA");
    return std::string(base ? base : ".") + "/soundboard";
#else
    const char* base = std::getenv("HOME");
    return std::string(base ? base : ".") + "/.config/soundboard";
#endif
}
inline std::string configPath = configDir() + "/config.json";
enum Tab{
    Soundboard,
    Mic,
    Settings,
    None
};
inline Tab currentTab = Soundboard;

enum btn_Group{
    Tab_Group
};

inline float scrollOffset = 0.0f;
inline int SoundID = 0;

inline bool hotkeysEnabled = true;
inline std::string soundsFolder;
inline int monitorMode = 2;
inline std::string inputDevice;
inline std::string outputDevice;
inline std::string virtualDevice;
}
