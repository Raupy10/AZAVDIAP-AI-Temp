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
            addSound("New Sound",sound["key"],sound["path"],false);
        }
        }
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


    for (auto it = config["sounds"].begin(); it != config["sounds"].end(); ++it)
    {
        if ((*it)["id"] == id)
        {
            config["sounds"].erase(it);
            break;
        }
    }


    std::ofstream out(gerneral::configPath);
    out << config.dump(4);
}
}