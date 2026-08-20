#include "ButtonManager.h"
#include "Button.h"
#include "SoundBoard/SoundboardManager.h"
#include "general.h"
#include <cstddef>
#include <raylib.h>
#include <string>

void InitButtons(){
    Button Soundboard_btn(gerneral::btn_Group::Tab_Group,"Soundboard_Tab",Soundboard_Icon,2,0,180,gerneral::Tab::Soundboard);
    Button Mic_btn(gerneral::btn_Group::Tab_Group,"Mic_Tab",Mic_Icon,2,0,230,gerneral::Tab::Mic);
    Button Settings_btn(gerneral::btn_Group::Tab_Group,"Settings_Tab",Settings_Icon,2,0,400,gerneral::Tab::Settings);


    Button AddSound_btn(gerneral::btn_Group::Soundboard_Group,"AddSound",60,gerneral::AddSoundButtonY,Vector2{730,50},0.5);

    buttonList.push_back(Soundboard_btn);
    buttonList.push_back(Mic_btn);
    buttonList.push_back(Settings_btn);
    buttonList.push_back(AddSound_btn);
}
void AddButton(gerneral::btn_Group group, Texture2D tex, int x, int y,float scale, std::string name,int counter){
    Button newButton(group,tex,x,y,scale,name,counter);
    buttonList.push_back(newButton);
}
void RemoveButton(const int& id)
{
    int Location = 0;
    for (auto it = buttonList.begin(); it != buttonList.end(); )
    {
        if (it->GetCounter() == id)
        {
            it = buttonList.erase(it);
            Location = id;
        }
        else
        {
            ++it;
        }
    }
    for(auto& b :buttonList)
    {
        if(b.GetCounter() > id){
            b.SetCounter(b.GetCounter()-1);
        }
    }
}
void DrawTabButtons(){
    for(auto& b : buttonList){
        if(b.GetGroup() == gerneral::btn_Group::Tab_Group)
            b.Draw();
    }
}
void DrawSoundboardButton(){
    for(auto& b : buttonList){
        if(b.GetGroup() == gerneral::btn_Group::Soundboard_Group)
            b.Draw();
    }
}
void UpdateSoundboardButton(){
    for(auto& b : buttonList){
        if(b.GetGroup() == gerneral::btn_Group::Soundboard_Group)
            if(b.GetName() =="AddSound")
            {
                b.UpdateY(60 + (counter * 60.0f) - gerneral::scrollOffset);
            }
            else{
                b.UpdateY(b.GetY() +(b.GetCounter() * 60.0f) - gerneral::scrollOffset);
            }
    }
}

void UpdateButtons(){
        UpdateSoundboardButton();

        for(auto& b : buttonList){
         bool res = b.Update(GetMousePosition());
         if(res)
         {
            if(b.GetGroup() == gerneral::btn_Group::Tab_Group) gerneral::currentTab = b.GetTab();
            PressedButton = &b;
            return;
         }
        }
        PressedButton = nullptr;

        
}
void UpdateButtonPosY(float y, std::string buttonname){
    for(auto& b : buttonList){
        if(b.GetName() == buttonname) b.UpdateY(y);
    }
}