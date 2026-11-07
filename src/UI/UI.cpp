#include "UI.h"
#include "Theme.h"
#include "Widgets.h"
#include "OpenFileDialog.h"
#include "../audio/AudioEngine.h"
#include "../platform/Platform.h"
#include "../core/Hotkeys.h"
#include "../core/LoadJson.h"
#include <algorithm>
#include <raylib.h>

void SoundboardUI(float T_StartX,float T_StartY);
void MicUI(float T_StartX,float T_StartY);
void SettingsUI(float T_StartX,float T_StartY);

namespace {

void Header(float x, float y, const char* title){
    DrawRectangleLines(x, y, 750, 50, DARKGRAY);
    DrawText(title, x + 5, y + 15, 25, DARKGRAY);
}

void ApplyAudio(){
    Audio::Start(gerneral::inputDevice, gerneral::outputDevice, gerneral::virtualDevice);
    LoadJson::SaveGeneral();
}

int IndexOf(const std::vector<std::string>& names, const std::string& stored, bool virt){
    if(stored.empty()) return 0;
    if(virt && stored == Audio::kNone) return 1;
    for(size_t i = 0; i < names.size(); i++)
        if(names[i] == stored) return (int)i;
    return 0;
}

bool Select(float y, const char* label, const std::vector<std::string>& names, std::string& stored, bool virt){
    UILabel(label, 70, y);
    int idx = IndexOf(names, stored, virt);
    int d = UISelector({70, y + 22, 700, 30}, names[idx]);
    if(d == 0) return false;
    int n = (int)names.size();
    idx = (idx + d + n) % n;
    if(idx == 0) stored = "";
    else if(virt && idx == 1) stored = Audio::kNone;
    else stored = names[idx];
    return true;
}

}

void RenderUI()
{
    static gerneral::Tab previous = gerneral::Tab::None;
    if(gerneral::currentTab != previous){
        if(previous == gerneral::Tab::Mic) Audio::SetInputTest(false);
        if(gerneral::currentTab == gerneral::Tab::Mic){
            Audio::RefreshDevices();
            if(!Audio::HasVirtual()) ApplyAudio();
        }
        previous = gerneral::currentTab;
    }

    DrawRectangleLines(0, 0, 50, 450, Color_Line);
    DrawRectangleLines(50, 0, 750, 450, Color_Line);

    DrawTabButtons();
    float T_StartX = 50;
    float T_StartY = 0;

    switch(gerneral::currentTab){
        case gerneral::Tab::Soundboard:
            SoundboardUI(T_StartX,T_StartY);
            break;
        case gerneral::Tab::Mic:
            MicUI(T_StartX,T_StartY);
            break;
        case gerneral::Tab::Settings:
            SettingsUI(T_StartX,T_StartY);
            break;
        default:
        break;
    }
}

void SoundboardUI(float T_StartX,float T_StartY){
    Header(T_StartX, T_StartY, "Soundboard");

    BeginScissorMode(T_StartX, T_StartY + 50, 750, 400);
        RenderSoundboard();
    EndScissorMode();
}

void MicUI(float T_StartX,float T_StartY){
    Header(T_StartX, T_StartY, "Mikrofon & Ausgabe");

    bool changed = false;
    changed |= Select(60, "Eingabegerät (Mikrofon)", Audio::InputNames(), gerneral::inputDevice, false);
    changed |= Select(125, "Ausgabegerät (Kopfhörer / Lautsprecher)", Audio::OutputNames(), gerneral::outputDevice, false);
    changed |= Select(190, "Virtuelles Mikrofon (geht an das Spiel, du h\u00f6rst es nicht)", Audio::VirtualNames(), gerneral::virtualDevice, true);
    if(changed) ApplyAudio();

    bool testing = Audio::InputTest();
    if(UIButton({70, 260, 170, 36}, testing ? "Test beenden" : "Eingabe testen", testing)) Audio::SetInputTest(!testing);
    if(UIButton({250, 260, 170, 36}, "Ausgabe testen")) Audio::PlayTestTone();

    Rectangle bar = {440, 270, 330, 16};
    DrawRectangleRec(bar, UI_Panel);
    DrawRectangleLinesEx(bar, 1, LIGHTGRAY);
    float lvl = std::min(1.0f, Audio::InputLevel() * 1.5f);
    DrawRectangleRec({bar.x + 1, bar.y + 1, (bar.width - 2) * lvl, bar.height - 2}, lvl > 0.85f ? RED : GREEN);

    UILabel("Ich höre:", 70, 315);
    static const char* labels[4] = {"Mich + Sounds", "Nur Sounds", "Nur mich", "Nichts"};
    static const int modes[4] = {3, 2, 1, 0};
    int sel = 3;
    for(int i = 0; i < 4; i++) if(modes[i] == gerneral::monitorMode) sel = i;
    int next = UISegment({70, 337, 700, 36}, labels, 4, sel);
    if(next != sel){
        gerneral::monitorMode = modes[next];
        Audio::SetMonitorMode(gerneral::monitorMode);
        LoadJson::SaveGeneral();
    }

    if(Audio::HasVirtual()){
        UILabel(UIFit("Im Spiel als Mikrofon w\u00e4hlen: " + Audio::VirtualMic(), 90), 70, 385, 16, GREEN);
        UILabel(UIFit(Audio::Status(), 100), 70, 410, 14, LIGHTGRAY);
    }
    else{
        bool failed = Audio::Status().rfind("Fehler", 0) == 0;
        UILabel(failed ? Audio::Status() : "Kein virtuelles Mikrofon gefunden - das Spiel h\u00f6rt dich so nicht.", 70, 385, 16, ORANGE);
        if(UIButton({70, 408, 210, 30}, "VB-CABLE herunterladen")) Platform::OpenUrl("https://vb-audio.com/Cable/");
        UILabel("Nach der Installation Windows neu starten.", 292, 416, 14, LIGHTGRAY);
    }
}

void SettingsUI(float T_StartX,float T_StartY){
    Header(T_StartX, T_StartY, "Einstellungen");

    UILabel("Globale Hotkeys", 70, 74);
    if(UIToggle({260, 70, 56, 26}, gerneral::hotkeysEnabled)){
        gerneral::hotkeysEnabled = !gerneral::hotkeysEnabled;
        Hotkeys::SetEnabled(gerneral::hotkeysEnabled);
        LoadJson::SaveGeneral();
    }
    UILabel(gerneral::hotkeysEnabled ? "An" : "Aus", 328, 74);

    UILabel("Ordner für Sounds", 70, 130);
    Rectangle box = {70, 152, 560, 34};
    DrawRectangleRec(box, UI_Panel);
    DrawRectangleLinesEx(box, 1, LIGHTGRAY);
    UILabel(gerneral::soundsFolder.empty() ? "(kein Ordner gewählt)" : UIFit(gerneral::soundsFolder, 68), box.x + 6, box.y + 9);
    if(UIButton({640, 152, 130, 34}, "Wählen")){
        std::string dir = OpenFolderDialog(gerneral::soundsFolder);
        if(!dir.empty()){
            gerneral::soundsFolder = dir;
            LoadJson::SaveGeneral();
        }
    }

    static std::string scanMessage;
    if(UIButton({70, 210, 200, 36}, "Nach MP3 suchen")){
        int n = ScanSoundsFolder();
        scanMessage = n < 0 ? "Kein gültiger Ordner gewählt" : std::to_string(n) + " neue Sounds hinzugefügt";
    }
    UILabel(scanMessage, 285, 220);

    UILabel("Stream Deck: Aktion \"Hotkey\" auf einen Knopf legen und denselben Hotkey (z.B. F13) senden.", 70, 400, 14, LIGHTGRAY);
}
