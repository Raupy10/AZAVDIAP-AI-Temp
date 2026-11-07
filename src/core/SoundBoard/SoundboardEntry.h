#pragma once
#include "../../UI/Theme.h"
#include "../../UI/TextBox.h"
#include "../../UI/Widgets.h"
#include "../../UI/OpenFileDialog.h"
#include "../../audio/AudioEngine.h"
#include "../general.h"
#include <string>
#include <raylib.h>

class SoundboardEntry{

    private:
    static constexpr float x = 60;
    static constexpr float width = 730;
    static constexpr float height = 50;

    int id;
    std::string path;
    TextBox keyBox;
    TextBox soundBox;

    public:
    enum class Result { Idle, Changed, Remove };
    inline static int activeKeyId = -1;

    SoundboardEntry(int id, const std::string& key, const std::string& path);
    Result Update(float y);
    void Draw(float y);
    int GetID() const;
    const std::string& GetPath() const;
    std::string GetKey() const;
    void SetPath(const std::string& path);
};
