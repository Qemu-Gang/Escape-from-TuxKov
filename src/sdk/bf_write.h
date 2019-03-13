#pragma once
#include "BaseStruct.h"
#include "Vector.h"

#include <cstdint>

class bf_write
{
public:
	void* m_data; //0x0000
	int32_t m_dataBytes; //0x0008
	int32_t m_dataBits; //0x000C
	int32_t m_curBit; //0x0010
	bool m_overflow; //0x0014
	bool m_assertOnOverflow; //0x0015
	char pad_0016[2]; //0x0016
	char* m_debugName; //0x0018
}; //Size: 0x0020