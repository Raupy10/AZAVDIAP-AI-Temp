#include "Button.h"
#include "general.h"
#include <raylib.h>

Button::Button(gerneral::btn_Group group,std::string name,Texture2D texture,float scale,int x,int y,gerneral::Tab tab){
    this->group = group;
    this->name = name;
    this->texture = texture;
    this->scale = scale;
    this->size = Vector2{(float)texture.width * scale,(float)texture.height * scale};
    this->x = x;
    this->y = y;
    this->tab = tab;
    this->rounding = 0;
}
Button::Button(gerneral::btn_Group group,std::string name,int x,int y,Vector2 size,float rounding){
    this->group = group;
    this->name = name;
    this->x = x;
    this->y = y;
    this-> size = size;
    this->rounding = rounding;
}
Button::Button(gerneral::btn_Group group,Texture2D tex,int x,int y,float scale,std::string name,int counter, int id){
    this->group = group;
    this->name = name;
    this->texture = tex;
    this-> scale = scale;
    this->size = Vector2{(float)texture.width * scale,(float)texture.height * scale};
    this->x = x;
    this->y = y;
    this->preScaleY = y;
    this->rounding = 0;
    this->counter = counter;
    this->id = id;
}


void Button::Draw(){
    if(hovered)
        DrawRectangleRounded(Rectangle{x,y,size.x,size.y},rounding,1,GRAY);
    float tempx = pressed ? x + ((size.x - (size.x * 0.8f))/2) : x;
    float tempy = pressed ? y + ((size.y - (size.y * 0.8f))/2) : y;
    DrawTextureEx(texture, (Vector2){tempx, tempy}, 0.0f,  pressed ? scale * 0.8f: scale , WHITE);
  
}
gerneral::Tab Button::GetTab(){
    return tab;
}
gerneral::btn_Group Button::GetGroup(){
    return group;
}
std::string Button::GetName(){
    return name;
}
bool Button::Update(Vector2 mousePos){
    
    if(CheckCollisionPointRec(mousePos, Rectangle{x,y,size.x,size.y}))
    {
        hovered = true;
        if(IsMouseButtonDown(MOUSE_LEFT_BUTTON))
        {
            pressed = true;
        }
        else{
            pressed = false;
        }   
        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) return true;
    }
    else{
        hovered = false;
        pressed = false;
    }
    return false;
}
void Button::UpdateY(float y){
    this->y = y;
}
float Button::GetY(){
    return preScaleY;
}
int Button::GetCounter(){
    return counter;
}
void Button::SetCounter(const int& counter){
    this->counter = counter;
}
int Button::GetID()
{
    return id;
}
void Button::SetID(const int& id)
{
    this->id =id;
}