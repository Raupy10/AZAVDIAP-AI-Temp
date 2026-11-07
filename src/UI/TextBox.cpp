#include "TextBox.h"


TextBox::TextBox(Rectangle rect, const std::string& placeholder, bool isKeyBox)
    : rect(rect), text(placeholder), name(placeholder), active(false), isKeyBox(isKeyBox) {}

void TextBox::Update(bool isActive)
{
    active = isActive;
    if (!active) return;

    if (isKeyBox)
    {
        if (IsKeyPressed(KEY_BACKSPACE) || IsKeyPressed(KEY_DELETE)) {
            Clear();
            return;
        }

        std::string mods = "";
        if (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) mods += "Ctrl+";
        if (IsKeyDown(KEY_LEFT_ALT)     || IsKeyDown(KEY_RIGHT_ALT))     mods += "Alt+";
        if (IsKeyDown(KEY_LEFT_SHIFT)   || IsKeyDown(KEY_RIGHT_SHIFT))   mods += "Shift+";

        for (int i = 0; i < 24; i++) {
            if (IsKeyPressed(KEY_F1 + i)) {
                text = mods + "F" + std::to_string(i + 1);
                return;
            }
        }
        for (int key = KEY_A; key <= KEY_Z; key++) {
            if (IsKeyPressed(key)) {
                text = mods + (char)('A' + (key - KEY_A));
                return;
            }
        }
        for (int key = KEY_ZERO; key <= KEY_NINE; key++) {
            if (IsKeyPressed(key)) {
                text = mods + (char)('0' + (key - KEY_ZERO));
                return;
            }
        }
    }
}

void TextBox::Draw()
{
    DrawRectangleRec(rect, active ? colorActive : colorInactive);
    DrawRectangleLinesEx(rect, 1, active ? WHITE : colorBorder);
    DrawText(text.c_str(), (int)rect.x + 6, (int)rect.y + 9, fontSize, colorText);

    if (active && (int)(GetTime() * 2) % 2 == 0) {
        int textW = MeasureText(text.c_str(), fontSize);
        DrawText("|", (int)rect.x + 6 + textW, (int)rect.y + 9, fontSize, colorText);
    }
}

bool TextBox::IsClicked()
{
    return IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), rect);
}