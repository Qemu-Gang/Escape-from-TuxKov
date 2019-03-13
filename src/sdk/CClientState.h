#pragma once

#include "BaseStruct.h"
#include "Vector.h"
#include "UtlMemory.h"
#include "bf_write.h"

#include <cstdint>

class CNetChan;

class CClientState
{
public:
	char pad_0000[96]; //0x0000
	class CNetChan* m_netChan; //0x0060
	char pad_0068[48]; //0x0068
	int32_t m_signonState; //0x0098
	char pad_009C[4]; //0x009C
	double m_nextCmdTime; //0x00A0
	uint32_t m_serverCount; //0x00A8
	uint32_t m_CurrentSequence; //0x00AC
	char pad_00B0[168]; //0x00B0
	int32_t m_deltaTick; //0x0158
	char pad_015C[12]; //0x015C
	uint32_t m_playerSlot; //0x0168
	char pad_016C[8]; //0x016C
	char m_mapFileName[64]; //0x0174
	char m_levelNameShort[64]; //0x01B4
	char m_levelName[260]; //0x01F4
	char pad_02F8[4]; //0x02F8
	int32_t m_highestClientIndex; //0x02FC
	char pad_0300[64]; //0x0300
	int32_t m_maxClients; //0x0340
	char pad_0344[100752]; //0x0344
	int32_t m_lastOutGoingCommand; //0x18CD4
	char pad_18CD8[64]; //0x18CD8
	vec3 m_viewangles; //0x18D18
}; //Size: 0x18D24

class CNetChan
{
public:
	char pad_0000[4]; //0x0000
	uint32_t m_outSeqNr; //0x0004
	uint32_t m_inSeqNr; //0x0008
	uint32_t m_outSeqNrAck; //0x000C
	uint32_t m_chokedCommands; //0x0010
	char pad_0014[16]; //0x0014
	int32_t m_socket; //0x0024
	char pad_0028[8]; //0x0028
	bf_write m_streamReliable; //0x0030
	CUtlMemory m_reliableDataBuffer; //0x0050
	bf_write m_streamUnreliable; //0x0068
	CUtlMemory m_unreliableDataBuffer; //0x0088
	bf_write m_streamVoice; //0x00A0
	CUtlMemory m_voiceDataBuffer; //0x00C0
	char pad_00D8[4]; //0x00D8
	uint32_t m_maxReliablePayloadSize; //0x00DC Not entirely sure about this one, 1200 seems low? idk.
	double m_lastReceived; //0x00E0
	double m_connectTime; //0x00E8
	int32_t m_rate; //0x00F0
	char pad_00F4[4]; //0x00F4
	double m_clearTime; //0x00F8
	char pad_0100[8]; //0x0100
	int32_t m_signonState; //0x0108 only display 4 and 8
	char pad_010C[108]; //0x010C
	class CClientState* m_clientState; //0x0178
	char pad_0180[64]; //0x0180
	void* m_sendBuffer; //0x01C0
	bf_write m_sendStream; //0x01C8
}; //Size: 0x01E8