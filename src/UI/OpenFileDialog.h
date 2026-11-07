#pragma once
#include "../platform/Platform.h"
#include <string>

inline std::string OpenFileDialog(const std::string& startDir = "") {
    return Platform::PickFile(startDir);
}

inline std::string OpenFolderDialog(const std::string& startDir = "") {
    return Platform::PickFolder(startDir);
}
