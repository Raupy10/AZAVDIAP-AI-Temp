#include "SoundboardEntry.h"
#include <raylib.h>

SoundboardEntry::SoundboardEntry(int id, const std::string& key, const std::string& path)
    : id(id),
      keyBox({ x + 60, 0, 180, 35 }, "set key", true),
      soundBox({ x + 250, 0, 400, 35 }, "set sound", false)
{
    if(!key.empty()) keyBox.SetText(key);
    SetPath(path);
}

void SoundboardEntry::SetPath(const std::string& p){
    path = p;
    if(p.empty()){
        soundBox.Clear();
        return;
    }
    soundBox.SetText(UIFit(fs::u8path(p).filename().u8string(), 44));
}

int SoundboardEntry::GetID() const { return id; }
const std::string& SoundboardEntry::GetPath() const { return path; }
std::string SoundboardEntry::GetKey() const { return keyBox.IsEmpty() ? "" : keyBox.GetText(); }

SoundboardEntry::Result SoundboardEntry::Update(float y){
    Result result = Result::Idle;
    keyBox.UpdateY(y);
    soundBox.UpdateY(y);

    Vector2 m = GetMousePosition();
    Rectangle play = {70, y + 7, 36, 36};
    Rectangle trash = {728, y + 7, 36, 36};

    if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && m.y >= 50){
        if(keyBox.IsClicked()){
            activeKeyId = id;
        }
        else{
            if(activeKeyId == id) activeKeyId = -1;
            if(soundBox.IsClicked()){
                std::string p = OpenFileDialog(gerneral::soundsFolder);
                if(!p.empty()){
                    SetPath(p);
                    result = Result::Changed;
                }
            }
            else if(CheckCollisionPointRec(m, play)){
                if(Audio::IsPlaying(path)) Audio::Stop(path);
                else Audio::Play(path);
            }
            else if(CheckCollisionPointRec(m, trash)){
                Audio::Stop(path);
                result = Result::Remove;
            }
        }
    }

    std::string before = keyBox.GetText();
    keyBox.Update(activeKeyId == id);
    if(keyBox.GetText() != before) result = Result::Changed;
    if(activeKeyId == id && (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_ESCAPE))) activeKeyId = -1;

    return result;
}

void SoundboardEntry::Draw(float y){
    DrawRectangleRoundedLines({x, y, width, height}, 0.5, 1, DARKGRAY);
    keyBox.Draw();
    soundBox.Draw();

    Rectangle play = {70, y + 7, 36, 36};
    Rectangle trash = {728, y + 7, 36, 36};
    Texture2D playTex = Audio::IsPlaying(path) ? Circle_pause_Icon : Circle_play_Icon;
    if(UIHover(play)) DrawRectangleRounded(play, 0.5, 1, GRAY);
    if(UIHover(trash)) DrawRectangleRounded(trash, 0.5, 1, GRAY);
    DrawTextureEx(playTex, Vector2{play.x + 4, play.y + 4}, 0.0f, 1.2f, WHITE);
    DrawTextureEx(Trash_Icon, Vector2{trash.x + 4, trash.y + 4}, 0.0f, 1.2f, WHITE);
}
