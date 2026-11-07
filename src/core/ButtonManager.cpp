#include "ButtonManager.h"
#include "Button.h"
#include "general.h"
#include <raylib.h>

void InitButtons(){
    buttonList.clear();
    buttonList.push_back(Button(gerneral::btn_Group::Tab_Group,"Soundboard_Tab",Soundboard_Icon,2,0,180,gerneral::Tab::Soundboard));
    buttonList.push_back(Button(gerneral::btn_Group::Tab_Group,"Mic_Tab",Mic_Icon,2,0,230,gerneral::Tab::Mic));
    buttonList.push_back(Button(gerneral::btn_Group::Tab_Group,"Settings_Tab",Settings_Icon,2,0,400,gerneral::Tab::Settings));
}

void DrawTabButtons(){
    for(auto& b : buttonList) b.Draw();
}

void UpdateButtons(){
    for(auto& b : buttonList){
        if(b.Update(GetMousePosition())) gerneral::currentTab = b.GetTab();
    }
}
