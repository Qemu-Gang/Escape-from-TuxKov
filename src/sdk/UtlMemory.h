#pragma once

#include <cstdint>

// lol wer relaly dont need all the funcs etc since we are external and cant call them anyways
class CUtlMemory
{
public:
	void* m_memory; //0x0000
	int64_t m_allocationCount; //0x0008
	int64_t m_growSize; //0x0010
}; //Size: 0x0018