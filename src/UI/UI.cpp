#include "UI.h"
#include "Theme.h"
#include <raylib.h>

//800 450
void SoundboardUI(float T_StartX,float T_StartY);
void DrawOverlay();

void RenderUI()
{
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
            DrawText("Mic", 300, 200, 20, DARKGRAY);
            break;
        case gerneral::Tab::Settings:
            DrawText("Settings", 300, 200, 20, DARKGRAY);
            break;
        default:
        break;
    }
}


void SoundboardUI(float T_StartX,float T_StartY){
    DrawRectangleLines(T_StartX, T_StartY, 750, 50, DARKGRAY);
    DrawText("Soundboard", T_StartX +5,  T_StartY +15,25, DARKGRAY);
    
    if (GetMouseWheelMove() != 0) {
        gerneral::scrollOffset -= GetMouseWheelMove() * 30.0f;
        if (gerneral::scrollOffset < 0) gerneral::scrollOffset = 0;
    }

    float maxScroll = (gerneral::SoundID * 60.0f) - 300.0f;
    if (maxScroll < 0) maxScroll = 0;
    if (gerneral::scrollOffset > maxScroll) gerneral::scrollOffset = maxScroll;

   BeginScissorMode(T_StartX, T_StartY + 50, 750, 400);

        DrawSoundboardButton();
        RenderSoundboard();

    EndScissorMode();

}

