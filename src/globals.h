#pragma once

#include "vmread/hlapi/hlapi.h"
#include <vector>
#include <cstdint>
#include <deque>
#include <map>
#include <linux/input-event-codes.h>

#include "sdk/MapHeader.h"

inline WinProcess *process;
inline uintptr_t mccBase;
inline uintptr_t haloReachBase;
inline uintptr_t metaMagic;
inline uintptr_t g_tag_addresses;
inline uintptr_t currentMapAddr;
inline uintptr_t currentMapStringIDTable;

inline SectionPointerConverter *debugPointerConverter;
inline SectionPointerConverter *resourcePointerConverter;
inline SectionPointerConverter *tagBufferPointerConverter;
inline SectionPointerConverter *localePointerConverter;
inline MetaAddressConverter *metaArea;

inline bool pressedKeys[500]; // keyboard is 0-256 and mouse is > 256, so let's make the array unreasonably big to avoid overwriting other data

inline bool running = true;