#pragma once
#include "raylib.h"
#include <string>
#include "iostream"

class TextBox {
public:
    TextBox(Rectangle rect, const std::string& placeholder = "", bool isKeyBox = false);
    void Update(bool isActive);
    void Draw();
    bool IsClicked();
    std::string GetText() const { return text; }
    void SetText(std::string t) { text = t; }
    bool IsActive() const { return active; }
    void UpdateY(float y) { rect.y = y +7; }
    int GetId() { return id; }
    void SetId(int id) { this->id = id; }
    std::string GetName() { return name; }
    bool IsEmpty() const { return text == name; }
    void Clear() { text = name; }

private:
    Rectangle rect;
    std::string text;
    std::string name;
    bool active;
    bool isKeyBox;
    int id = 0;
    Color colorActive   = { 80, 80, 80, 255 };
    Color colorInactive = { 60, 60, 60, 255 };
    Color colorBorder   = LIGHTGRAY;
    Color colorText     = RAYWHITE;
    int fontSize        = 16;
    int maxChars        = 20;
};