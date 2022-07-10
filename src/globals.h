#pragma once

#include "sdk/unity.h"
#include "vmread/hlapi/hlapi.h"
#include <vector>
#include <cstdint>
#include <deque>
#include <map>
#include <atomic>
#include <linux/input-event-codes.h>

inline WinProcess *process;
inline uintptr_t unityplayerBase;
inline uintptr_t monoBase;
inline uintptr_t monoRootDomain;
inline uintptr_t gomAddr;
inline GlobalObjectManager gom;
inline uintptr_t gameWorldAddr;
inline uintptr_t cameraAddr;
inline uintptr_t localPlayerAddr;
inline Vector3D localPlayerHead;
inline bool localPlayerIsAiming;
inline bool localPlayerIsOpticCamera;
inline CameraEntity cameraEntity;

inline bool pressedKeys[500]; // keyboard is 0-256 and mouse is > 256, so let's make the array unreasonably big to avoid overwriting other data
inline std::atomic<bool> running = true;
inline bool mainLoopDone = true;