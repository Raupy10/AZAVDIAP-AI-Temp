#include <cstdlib>
#include <filesystem>
#include <stdlib.h>
#include <string>
#include <unistd.h>


#include "raylib.h"
#include "src/UI/Theme.h"
#include "src/UI/UI.h"
#include "src/core/ButtonManager.h"
#include "src/core/SoundBoard/SoundboardManager.h"
#include "src/core/ButtonCheck.h"
#include "src/core/LoadJson.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 450

void checkConfig();

int main()
{   
    checkConfig();


    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "AZAVDIAP");
    SetTargetFPS(60);

    LoadTextures();
    InitButtons();
    InitSoundboardAddButton();
    LoadJson::Load();

    Color Color_BG = {44, 47, 54, 255};

    char filepath[512];
    char command[1024];

    getcwd(filepath, sizeof(filepath));



    while (!WindowShouldClose())
    {
        //Update
        UpdateButtons();
        CheckButtons();
        UpdateSoundBoardBoxes();

        //Draw
        BeginDrawing();

        ClearBackground(Color_BG);

        RenderUI();

        EndDrawing();
    }

    CloseWindow();
    return 0;
}


void checkConfig()
{
    std::string config =
        std::string(getenv("HOME")) +
        "/.config/soundboard/config.json";

    if (fs::exists(config))
        return;

    fs::create_directories(
        std::string(getenv("HOME")) +
        "/.config/soundboard"
    );

    fs::copy_file(
        "default_config.json",
        config
    );
}