#pragma once
#include "SoundboardEntry.h"
#include <string>
#include <vector>
#include "../general.h"
#include "../ButtonManager.h"
#include "../../UI/Theme.h"

inline std::vector<SoundboardEntry> Sounds;
inline std::string ButtonName;
inline float yPosAddButton;
inline int SoundCountOffset = 0;


void addSound(std::string name,std::string text,std::string path,bool IsnewSound, int id);
void RenderSoundboard();
void CheckButtonPressed();
void InitSoundboardAddButton();
void UpdateSoundBoardBoxes();
void LoadJsonSounds();
std::string GetButtonPressedName();
void RemoveEntry(const int& id);

