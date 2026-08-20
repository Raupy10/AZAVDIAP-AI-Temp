#pragma once
#include "string.h"
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
    bool hovered,pressed;
    gerneral::Tab tab;
    gerneral::btn_Group group;
    std::string name;
    int id;
    int counter;
    int preScaleY;

    float rounding;


    


    public:
    Button(gerneral::btn_Group group,std::string name,Texture2D texture,float scale,int x,int y,gerneral::Tab tab);
    Button(gerneral::btn_Group group,std::string name,int x,int y,Vector2 size, float rounding);
    Button(gerneral::btn_Group group,Texture2D tex, int x,int y,float scale,std::string name,int counter, int id);
    void Draw();
    void SetTexture(Texture2D texture);
    bool Update(Vector2 mousePos);
    gerneral::Tab GetTab();
    gerneral::btn_Group GetGroup();
    std::string GetName();
    float GetY();
    int GetCounter();
    void SetCounter(const int& counter);
    void UpdateY(float y);
    int GetID();
    void SetID(const int& id);

    
};