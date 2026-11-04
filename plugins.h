#pragma once
#include <cstdint>
#include "config.h"
uintptr_t FindModuleBase(const char* name);
void LoadPlugins(const SampConfig& cfg);
