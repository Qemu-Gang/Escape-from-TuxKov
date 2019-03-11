#pragma once

#include "vmread/hlapi/hlapi.h"
#include "sdk/CBaseEntity.h"
#include <vector>
#include <cstdint>

inline std::vector<size_t> sortedEntities;
inline CBaseEntity entities[100];

inline CBaseEntity localPlayer;
inline uintptr_t entList;
inline uintptr_t sendpacket;
inline uintptr_t globalVars;
inline uintptr_t netTime; // double
inline uintptr_t nextCmdTime; // double

inline WinProcess *process;
inline WinProcess *inputSystem;

inline uintptr_t inputBase;
inline uintptr_t apexBase;
