#pragma once
#include <string>
#include <vector>

namespace Hotkeys {

struct Bind {
    std::string combo;
    std::string path;
    bool stopAll = false;
};

void Start();
void Stop();
void SetBinds(const std::vector<Bind>& binds);
void SetEnabled(bool on);
void SetSuspended(bool on);

}
