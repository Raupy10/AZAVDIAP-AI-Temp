#include "LoadJson.h"

namespace LoadJson {

void Load(){
    cfg = json::object();
    std::ifstream in(gerneral::configPath);
    if(in){
        try { in >> cfg; } catch(...) { cfg = json::object(); }
    }
    if(!cfg.is_object()) cfg = json::object();
    if(!cfg.contains("sounds") || !cfg["sounds"].is_array()) cfg["sounds"] = json::array();
    if(!cfg.contains("general") || !cfg["general"].is_object()) cfg["general"] = json::object();

    json& g = cfg["general"];
    gerneral::SoundID = g.value("soundsID", 0);
    gerneral::hotkeysEnabled = g.value("hotkeys", true);
    gerneral::soundsFolder = g.value("soundsFolder", "");
    gerneral::monitorMode = g.value("monitorMode", 2);
    gerneral::inputDevice = g.value("inputDevice", "");
    gerneral::outputDevice = g.value("outputDevice", "");
    gerneral::virtualDevice = g.value("virtualDevice", "");

    for(auto& s : cfg["sounds"]) if(s.contains("id")) gerneral::SoundID = std::max<int>(gerneral::SoundID, s["id"].get<int>());

    if(!in) Save();
}

void Save(){
    std::error_code ec;
    fs::create_directories(fs::path(gerneral::configPath).parent_path(), ec);
    std::ofstream out(gerneral::configPath);
    out << cfg.dump(4);
}

void SaveGeneral(){
    json& g = cfg["general"];
    g["soundsID"] = gerneral::SoundID;
    g["hotkeys"] = gerneral::hotkeysEnabled;
    g["soundsFolder"] = gerneral::soundsFolder;
    g["monitorMode"] = gerneral::monitorMode;
    g["inputDevice"] = gerneral::inputDevice;
    g["outputDevice"] = gerneral::outputDevice;
    g["virtualDevice"] = gerneral::virtualDevice;
    Save();
}

}
