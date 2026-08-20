#pragma once
#include "ButtonManager.h"

#include "SoundBoard/SoundboardManager.h"
#include "LoadJson.h"
#include <cstddef>


inline void CheckButtons(){
    if(PressedButton == nullptr)
        return;
    if(PressedButton->GetName() == "AddSound"){
        addSound("New Sound","","",true);
    }
    if(PressedButton->GetName().find("Trash") != std::string::npos)
    {
        LoadJson::RemoveEntry(PressedButton->GetCounter());
        RemoveButton(PressedButton->GetCounter());
        RemoveEntry(PressedButton->GetCounter());


    }
    if(PressedButton->GetName().find("Play") != std::string::npos)
    {
        
    }

}