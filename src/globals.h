#pragma once

#include "vmread/hlapi/hlapi.h"
#include <vector>
#include <cstdint>

inline std::vector<uintptr_t> entities;

inline uintptr_t localPlayer;
inline uintptr_t entList;
inline uintptr_t sendpacket;
inline uintptr_t globalVars;

inline WinProcess *process;
inline WinProcess *inputSystem;

inline uintptr_t inputBase;
inline uintptr_t apexBase;