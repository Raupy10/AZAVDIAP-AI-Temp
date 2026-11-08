#include "raylib.h"
#include "src/UI/Theme.h"
#include "src/UI/UI.h"
#include "src/audio/AudioEngine.h"
#include "src/core/ButtonManager.h"
#include "src/core/Hotkeys.h"
#include "src/core/LoadJson.h"
#include "src/core/SoundBoard/SoundboardManager.h"
#include "src/platform/Platform.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 450

int main()
{
    if (Platform::AlreadyRunning()) return 0;

    LoadJson::Load();

    Audio::Init();
    Audio::SetMonitorMode(gerneral::monitorMode);
    Audio::SetRetriggerMode(gerneral::retriggerMode);
    Audio::Start(gerneral::inputDevice, gerneral::outputDevice, gerneral::virtualDevice);

    Hotkeys::SetEnabled(gerneral::hotkeysEnabled);
    Hotkeys::Start();

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "AZAVDIAP");
    SetTargetFPS(60);
    SetExitKey(KEY_NULL);
    Platform::TrayInit(GetWindowHandle(), "AZAVDIAP - l\u00e4uft im Hintergrund");

    LoadTextures();
    InitButtons();
    LoadSounds();

    Color Color_BG = {44, 47, 54, 255};

    while (!WindowShouldClose() && !Platform::TrayQuitRequested())
    {
        if (Platform::WindowHidden())
        {
            SoundboardEntry::activeKeyId = -1;
            gerneral::settingsKeyActive = false;
            Hotkeys::SetSuspended(false);
            Audio::SetInputTest(false);
            PollInputEvents();
            WaitTime(0.05);
            continue;
        }

        UpdateButtons();
        if (gerneral::currentTab == gerneral::Tab::Soundboard) UpdateSoundboard();
        else {
            SoundboardEntry::activeKeyId = -1;
            Hotkeys::SetSuspended(gerneral::settingsKeyActive);
        }

        BeginDrawing();
        ClearBackground(Color_BG);
        RenderUI();
        EndDrawing();
    }

    Platform::TrayShutdown();
    Hotkeys::Stop();
    Audio::Shutdown();
    CloseWindow();
    return 0;
}
