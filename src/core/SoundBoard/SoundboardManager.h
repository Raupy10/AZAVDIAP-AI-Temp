#pragma once
#include "SoundboardEntry.h"
#include <string>
#include <vector>
#include "../general.h"
#include "../../UI/Theme.h"

inline std::vector<SoundboardEntry> Sounds;

void LoadSounds();
void SaveSounds();
void SyncHotkeys();
void AddSound(const std::string& key, const std::string& path);
int ScanSoundsFolder();
void UpdateSoundboard();
void RenderSoundboard();
