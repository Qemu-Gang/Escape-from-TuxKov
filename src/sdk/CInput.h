#pragma once

#include "BaseStruct.h"
#include "Vector.h"

#include <cstdint>

class CUserCmd
{
public:
	uint32_t m_commandNumber; //0x0000
	uint32_t m_tickCount; //0x0004
	float m_curtime; //0x0008
	QAngle m_viewAngles; //0x000C
	QAngle m_aimDirection; //0x0018
	float m_forwardmove; //0x0024
	float m_sidemove; //0x0028
	float m_upmove; //0x002C
	uint32_t m_buttons; //0x0030
	int32_t m_impulse; //0x0034
	char pad_0038[332]; //0x0038
	uint32_t m_randomSeed; //0x0184
	char pad_0188[8]; //0x0188
	vec3 m_eyePos; //0x0190
	char pad_019C[60]; //0x019C
	uint32_t m_prevTickcount; //0x01D8
	uint32_t m_nextTickcount; //0x01DC
	char pad_01E0[8]; //0x01E0
	float m_frametime; //0x01E8
	char pad_01EC[12]; //0x01EC
}; //Size: 0x01F8


class CVerifiedUserCmd : public CUserCmd
{
public:
    uint64_t m_crc64; //0x01F8
}; //Size: 0x0200

class CInput
{
public:
    char pad_0000[32]; //0x0000
    float m_frametime; //0x0020
    float m_prevFrametime; //0x0024
    char pad_0028[136]; //0x0028
    uint32_t m_buttonBits; //0x00B0
    char pad_00B4[8]; //0x00B4
    float m_pingHoldStartTime; //0x00BC
    float m_somethingPing; //0x00C0
    char pad_00C4[11]; //0x00C4
    bool m_cameraIsOrthographic; //0x00CF
    Vector m_previousViewAngles; //0x00D0
    char pad_00DC[12]; //0x00DC
    float m_lastForwardMove; //0x00E8
    int32_t m_clearInputState; //0x00EC
    CUserCmd* m_commands; //0x00F0 // Does not change or go null
    CVerifiedUserCmd* m_verifiedCommands; //0x00F8 // Does not change or go null
}; //Size: 0x0100

