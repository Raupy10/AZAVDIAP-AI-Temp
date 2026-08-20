#pragma once
#include "general.h"
#include "SoundBoard/SoundboardManager.h"
#include "ButtonManager.h"
namespace LoadJson {
    void Load(){

    json config;
    std::ifstream in(gerneral::configPath);

    if(!in)
    {
        std::cout << "Keine Json gefunden path: "<< gerneral::configPath << std::endl;
    }
    else{
        in >> config;
    
        for(auto& sound : config["sounds"])
        {
            addSound("New Sound",sound["key"],sound["path"],false, sound["id"]);
        }
        }
        gerneral::SoundID =config["general"]["soundsID"];
        std::cout << gerneral::SoundID << std::endl;
    UpdateSoundboardButton();
    }


    void RemoveEntry(int id)
    {
    json config;

    std::ifstream in(gerneral::configPath);

    if (!in)
    {
        std::cout << "Keine Json gefunden path: "
                  << gerneral::configPath << std::endl;
        return;
    }

    in >> config;

    std::cout << "gesucht:" << id << std::endl;
    for (auto it = config["sounds"].begin(); it != config["sounds"].end(); ++it)
    {
        std::cout << (*it) ["id"] << std::endl;
        if ((*it)["id"] == id)
        {
            config["sounds"].erase(it);
            std::cout << "gefunden:" << (*it) ["id"] << std::endl;
            break;
        }
    }


    std::ofstream out(gerneral::configPath);
    out << config.dump(4);
}
}