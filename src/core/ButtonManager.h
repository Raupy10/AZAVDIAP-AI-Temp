#pragma once

#include "Button.h"
#include "../UI/Theme.h"
#include "general.h"
#include <string>
#include <vector>

void InitButtons();
void DrawTabButtons();
void UpdateButtons();

inline std::vector<Button> buttonList;
