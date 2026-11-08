#pragma once
#include "raylib.h"
#include "Theme.h"
#include <string>
#include <vector>

inline const Color UI_Panel  = {60, 60, 60, 255};
inline const Color UI_Hover  = {80, 80, 80, 255};
inline const Color UI_Accent = {70, 130, 180, 255};

inline bool UIHover(Rectangle r){
    return CheckCollisionPointRec(GetMousePosition(), r);
}

inline bool UIClicked(Rectangle r){
    return IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && UIHover(r);
}

inline void UILabel(const std::string& t, float x, float y, int size = 16, Color c = RAYWHITE){
    DrawText(t.c_str(), (int)x, (int)y, size, c);
}

inline std::string UIFit(const std::string& s, int maxChars){
    if((int)s.size() <= maxChars) return s;
    return s.substr(0, maxChars - 3) + "...";
}

inline std::vector<std::string> UIWrap(const std::string& s, size_t maxChars, size_t maxLines){
    std::vector<std::string> lines;
    std::string cur;
    size_t i = 0;
    while(i < s.size()){
        size_t j = s.find(' ', i);
        std::string word = s.substr(i, j == std::string::npos ? std::string::npos : j - i);
        i = j == std::string::npos ? s.size() : j + 1;
        if(!cur.empty() && cur.size() + 1 + word.size() > maxChars){
            lines.push_back(cur);
            cur.clear();
        }
        if(!cur.empty()) cur += ' ';
        cur += word;
        while(cur.size() > maxChars){
            lines.push_back(cur.substr(0, maxChars));
            cur = cur.substr(maxChars);
        }
    }
    if(!cur.empty()) lines.push_back(cur);
    if(lines.size() > maxLines) lines.resize(maxLines);
    return lines;
}

inline bool UIButton(Rectangle r, const std::string& text, bool active = false){
    DrawRectangleRec(r, active ? UI_Accent : (UIHover(r) ? UI_Hover : UI_Panel));
    DrawRectangleLinesEx(r, 1, LIGHTGRAY);
    int w = MeasureText(text.c_str(), 16);
    DrawText(text.c_str(), (int)(r.x + (r.width - w) / 2), (int)(r.y + (r.height - 16) / 2), 16, RAYWHITE);
    return UIClicked(r);
}

inline bool UIToggle(Rectangle r, bool on){
    DrawRectangleRounded(r, 1.0f, 8, on ? UI_Accent : UI_Panel);
    DrawRectangleRoundedLines(r, 1.0f, 8, LIGHTGRAY);
    float d = r.height - 6;
    DrawCircle((int)(on ? r.x + r.width - d / 2 - 3 : r.x + d / 2 + 3), (int)(r.y + r.height / 2), d / 2, RAYWHITE);
    return UIClicked(r);
}

inline int UISegment(Rectangle r, const char* const* labels, int n, int sel){
    float w = r.width / n;
    int result = sel;
    for(int i = 0; i < n; i++){
        if(UIButton({r.x + w * i, r.y, w, r.height}, labels[i], i == sel)) result = i;
    }
    return result;
}

inline int UISelector(Rectangle r, const std::string& text){
    int result = 0;
    if(UIButton({r.x, r.y, r.height, r.height}, "<")) result = -1;
    if(UIButton({r.x + r.width - r.height, r.y, r.height, r.height}, ">")) result = 1;
    Rectangle mid = {r.x + r.height + 4, r.y, r.width - 2 * r.height - 8, r.height};
    DrawRectangleRec(mid, UI_Panel);
    DrawRectangleLinesEx(mid, 1, LIGHTGRAY);
    UILabel(UIFit(text, (int)(mid.width / 8)), mid.x + 6, mid.y + (mid.height - 16) / 2);
    return result;
}
