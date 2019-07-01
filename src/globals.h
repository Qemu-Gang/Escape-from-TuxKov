#pragma once

#include "vmread/hlapi/hlapi.h"
#include "sdk/CBaseEntity.h"
#include "sdk/CInput.h"
#include "sdk/Definitions.h"
#include "sdk/CGlobalVars.h"
#include "sdk/CClientState.h"
#include <vector>
#include <cstdint>
#include <deque>
#include <map>
#include <linux/input-event-codes.h>

inline std::vector<size_t> validEntities;
inline CBaseEntity entities[100];

inline CBaseEntity localPlayer;
inline uintptr_t entList;
inline uintptr_t globalVarsAddr;
inline uintptr_t clientStateAddr;
inline uintptr_t inputAddr;
inline uintptr_t timescale;
inline uintptr_t forceJump;

inline uintptr_t userCmdArr;
inline uintptr_t verifiedUserCmdArr;

inline CGlobalVars globalVars;
inline CNetChan netChan;
inline CClientState clientState;

inline WinProcess *process;

inline uintptr_t apexBase;

inline uintptr_t EACGameClient;

inline bool pressedKeys[500]; // keyboard is 0-256 and mouse is > 256, so let's make the array unreasonably big to avoid overwriting other data

static bool running = true;

inline std::map<int, QAngle> sway_history;