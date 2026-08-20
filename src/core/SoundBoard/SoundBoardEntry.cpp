#include "SoundboardEntry.h"
#include "SoundboardManager.h"
#include <raylib.h>


SoundboardEntry::SoundboardEntry(std::string name, float y, int id){
    this->name = name;

    this->y = y;
    this->id = id;
    boxes[0].UpdateY(y);
    boxes[0].SetId(BoxIdCounter++);
    boxes[1].UpdateY(y);
    boxes[1].SetId(BoxIdCounter++);


}
SoundboardEntry::SoundboardEntry(float y){
    this->name = "button";
    this->y = y;

}

void SoundboardEntry::Draw(){
    drawY = y - gerneral::scrollOffset;
    Rectangle rec = {x,drawY,width,height};
    DrawRectangleRoundedLines(rec, 0.5, 1, DARKGRAY);
    if(name == "button"){
        DrawTextureEx(Add_Icon, Vector2{x+340,drawY +15},0.0f, 1.0f, WHITE);
    }
    else{
        boxes[0].Draw();
        boxes[1].Draw();
    }

}
std::string SoundboardEntry::GetName(){
    return name;
}
bool SoundboardEntry::isPressed(){
    Rectangle rec = {x,y,width,height};
    if(CheckCollisionPointRec(GetMousePosition(),  rec)){
        return true;
    }
    return false;
}
void SoundboardEntry::UpdateY(float y){
    this->y = y;
}
void SoundboardEntry::UpdateBoxes(){
    drawY = y - gerneral::scrollOffset;
    bool IsBoxPressed = false;
    if(name != "button")
    {
        boxes[0].UpdateY(drawY);
        boxes[1].UpdateY(drawY);
        if(gerneral::justAdded) return;
        for (int i = 0; i < (int)boxes.size(); i++){
            
            if (boxes[i].IsClicked()) 
            {
                IsBoxPressed = true;
                if(boxes[i].GetName() == "set key") activeBox = boxes[i].GetId();
                else if (boxes[i].GetName() == "set sound")
                {
                     std::string path = OpenFileDialog();
                    if(path != "")
                    {
                        std::string filename = path.substr(path.find_last_of("/") + 1);
                        boxes[i].SetText(filename);
                    }
                }
                 
            }
        }
        if(!IsBoxPressed && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {   
           activeBox = -1;
        }

        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_ESCAPE))
            activeBox = -1;

        for (int i = 0; i < (int)boxes.size(); i++)
        {
            boxes[i].Update(activeBox == boxes[i].GetId());
        }
    }
}
void SoundboardEntry::SetKey(std::string key){
 boxes[0].SetText(key);
}

void SoundboardEntry::SetPath(std::string path){
boxes[1].SetText(path);
}
int SoundboardEntry::GetCounter()
{
    return id;
}
void SoundboardEntry::SetCounter(const int& counter){
    this->id = counter;
}
void SoundboardEntry::UpdateY(){
    y -= 60;
}