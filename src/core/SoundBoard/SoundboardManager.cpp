#include "SoundboardManager.h"
#include "SoundboardEntry.h"
#include <fstream>
#include <iostream>
#include <string>

void addSound(std::string name,std::string text,std::string path, bool IsnewSound, int id){
    int tempy = 60 + (SoundCountOffset * 60); 
    gerneral::SoundID++;
    SoundCountOffset++;
    yPosAddButton = 60 +(SoundCountOffset * 60);

    SoundboardEntry newSound(name, tempy, gerneral::SoundID);
    Sounds.push_back(newSound);
    gerneral::justAdded = true;

    for( auto& s : Sounds)
    {
        if(s.GetName() == "button") s.UpdateY(60 +( SoundCountOffset * 60));
        UpdateButtonPosY((60+(SoundCountOffset * 60)), "AddSound");
    }

    
    json config;
    std::ifstream in(gerneral::configPath);

    if(!in)
    {
        std::cout << "Keine Json gefunden path: "<< gerneral::configPath << std::endl;
    }
    else{
        in >> config;

        if(IsnewSound)
        {
            config["sounds"].push_back({
            {"id", gerneral::SoundID},
            {"key", ""},
            {"path", ""},
            });
            AddButton(gerneral::btn_Group::Soundboard_Group, Trash_Icon, 730, 5, 1.5f, "Trash",SoundCountOffset,gerneral::SoundID);
            AddButton(gerneral::btn_Group::Soundboard_Group, Circle_play_Icon, 70, 12, 1.2f, "Play",SoundCountOffset,gerneral::SoundID);

            config["general"]["soundsID"] = gerneral::SoundID;

        std::ofstream out(gerneral::configPath);
        out << config.dump(4);
        }
        else{
            newSound.SetKey(text);
            newSound.SetPath(path);
            AddButton(gerneral::btn_Group::Soundboard_Group, Trash_Icon, 730, 5, 1.5f, "Trash",SoundCountOffset,id);
            AddButton(gerneral::btn_Group::Soundboard_Group, Circle_play_Icon, 70, 12, 1.2f, "Play",SoundCountOffset,id);
        }
        
        
        
    }
    
}

void RenderSoundboard(){
    for(auto& s : Sounds)
    {
        s.Draw();
    }
}
void CheckButtonPressed(){
    for(auto& s : Sounds)
    {
        if(s.isPressed())
        {
            ButtonName = s.GetName();
        }
    }
}
std::string GetButtonPressedName(){
    return ButtonName;
}

void InitSoundboardAddButton(){
    SoundboardEntry button(60);
    Sounds.push_back(button);
}
void UpdateSoundBoardBoxes(){
    for(auto& s: Sounds){
        s.UpdateBoxes();
    }
    gerneral::justAdded = false; 
}
void RemoveEntry(const int& id)
{
    SoundCountOffset--;
    int Location =0;
    for(auto it = Sounds.begin(); it < Sounds.end();it++)
    {
        if( it->GetCounter() == id)
        {
            Location = id;
            Sounds.erase(it);       
        }
    }
    for(auto& S :Sounds)
    {
        if(S.GetCounter() >Location)
        {
            S.SetCounter(S.GetCounter()-1);
            S.UpdateY();
        }
    }
    for( auto& s : Sounds)
    {
        if(s.GetName() == "button") s.UpdateY(60 +( SoundCountOffset * 60));
        UpdateButtonPosY((60+(SoundCountOffset * 60)), "AddSound");
    }
}