#pragma once
#include "SoundboardEntry.h"
#include <string>
#include <vector>
#include "../general.h"
#include "../ButtonManager.h"
#include "../../UI/Theme.h"

inline std::vector<SoundboardEntry> Sounds;
inline int counter = 0;
inline std::string ButtonName;
inline float yPosAddButton;


void addSound(std::string name,std::string text,std::string path,bool IsnewSound);
void RenderSoundboard();
void CheckButtonPressed();
void InitSoundboardAddButton();
void UpdateSoundBoardBoxes();
void LoadJsonSounds();
std::string GetButtonPressedName();
void RemoveEntry(const int& id);

