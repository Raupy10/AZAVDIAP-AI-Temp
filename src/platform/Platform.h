#pragma once
#include <string>

namespace Platform {

struct Combo {
    int key = 0;
    bool ctrl = false;
    bool alt = false;
    bool shift = false;
};

bool ParseCombo(const std::string& text, Combo& out);
bool KeysInit();
void KeysClose();
void SampleKeys();
bool ComboDown(const Combo& c);

std::string PickFile(const std::string& startDir);
std::string PickFolder(const std::string& startDir);
std::string RunCommand(const std::string& cmd);
void OpenUrl(const std::string& url);

bool AlreadyRunning();
bool TrayInit(void* nativeWindow, const char* tip);
void TrayShutdown();
bool TrayQuitRequested();
bool WindowHidden();

}
