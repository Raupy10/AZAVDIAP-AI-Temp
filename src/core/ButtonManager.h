#pragma once

#include "Button.h"
#include "../UI/Theme.h"
#include "general.h"
#include <string>
#include <vector>

void InitButtons();
void DrawTabButtons();
void DrawSoundboardButton();
void UpdateSoundboardButton();
void UpdateButtons();
void AddButton(gerneral::btn_Group group, Texture2D tex, int x, int y,float scale, std::string name,int counter);
void RemoveButton(const int& id);

void UpdateButtonPosY(float y,std::string buttonname);

inline std::vector<Button> buttonList;
inline Button* PressedButton;
