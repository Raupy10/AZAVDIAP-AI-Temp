#pragma once
#include "general.h"

#include <raylib.h>
#include <string>

class Button
{
    private:
    Texture2D texture;
    Vector2 size;
    float scale;
    float x,y;
    bool hovered = false;
    bool pressed = false;
    gerneral::Tab tab;
    gerneral::btn_Group group;
    std::string name;

    public:
    Button(gerneral::btn_Group group,std::string name,Texture2D texture,float scale,int x,int y,gerneral::Tab tab);
    void Draw();
    bool Update(Vector2 mousePos);
    gerneral::Tab GetTab();
    gerneral::btn_Group GetGroup();
    std::string GetName();
};
