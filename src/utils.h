#ifndef APE_EX_UTILS_H
#define APE_EX_UTILS_H

#include "vmread/hlapi/hlapi.h"
#include <unistd.h> //getpid
#include <thread>
#include <atomic>
#include <csignal>
//#include "sdk/QAngle.h"


inline std::vector<uintptr_t> entities;

inline uintptr_t localPlayer;

inline WinProcess *process;
inline WinProcess *inputSystem;

inline uintptr_t inputSystemBase;
inline uintptr_t apexLegendsBase;

#endif  // APE_EX_UTILS_H
