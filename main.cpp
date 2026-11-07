#include "raylib.h"
#include "src/UI/Theme.h"
#include "src/UI/UI.h"
#include "src/audio/AudioEngine.h"
#include "src/core/ButtonManager.h"
#include "src/core/Hotkeys.h"
#include "src/core/LoadJson.h"
#include "src/core/SoundBoard/SoundboardManager.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 450

int main()
{
    LoadJson::Load();

    Audio::Init();
    Audio::SetMonitorMode(gerneral::monitorMode);
    Audio::Start(gerneral::inputDevice, gerneral::outputDevice, gerneral::virtualDevice);

    Hotkeys::SetEnabled(gerneral::hotkeysEnabled);
    Hotkeys::Start();

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "AZAVDIAP");
    SetTargetFPS(60);

    LoadTextures();
    InitButtons();
    LoadSounds();

    Color Color_BG = {44, 47, 54, 255};

    while (!WindowShouldClose())
    {
        UpdateButtons();
        if (gerneral::currentTab == gerneral::Tab::Soundboard) UpdateSoundboard();
        else {
            SoundboardEntry::activeKeyId = -1;
            Hotkeys::SetSuspended(false);
        }

        BeginDrawing();
        ClearBackground(Color_BG);
        RenderUI();
        EndDrawing();
    }

    Hotkeys::Stop();
    Audio::Shutdown();
    CloseWindow();
    return 0;
}
