#include "Theme.h"
#include "EmbeddedAssets.h"
#include <raylib.h>

Texture2D Soundboard_Icon;
Texture2D Mic_Icon;
Texture2D Settings_Icon;
Texture2D Import_Icon;
Texture2D Add_Icon;
Texture2D Trash_Icon;
Texture2D Circle_play_Icon;
Texture2D Circle_pause_Icon;

const Color Color_BG   = {44, 47, 54, 255};
const Color Color_Line = {80, 80, 80, 255};
const Color Color_OverlayBG = {35, 41, 51,120};

static Texture2D LoadEmbedded(const unsigned char* data, int size)
{
    Image image = LoadImageFromMemory(".png", data, size);
    Texture2D texture = LoadTextureFromImage(image);
    UnloadImage(image);
    return texture;
}

void LoadTextures()
{
    Soundboard_Icon = LoadEmbedded(Asset_audio_lines, sizeof(Asset_audio_lines));
    Mic_Icon = LoadEmbedded(Asset_mic, sizeof(Asset_mic));
    Settings_Icon = LoadEmbedded(Asset_settings, sizeof(Asset_settings));
    Add_Icon = LoadEmbedded(Asset_circle_plus, sizeof(Asset_circle_plus));
    Trash_Icon = LoadEmbedded(Asset_trash, sizeof(Asset_trash));
    Circle_pause_Icon = LoadEmbedded(Asset_circle_pause, sizeof(Asset_circle_pause));
    Circle_play_Icon = LoadEmbedded(Asset_circle_play, sizeof(Asset_circle_play));
}
