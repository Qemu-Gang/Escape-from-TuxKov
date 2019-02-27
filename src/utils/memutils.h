#ifndef MEMUTILS_H
#define MEMUTILS_H

#include <string.h>

//External programs might want to use custom RPM/WPM functions
#ifndef MEMUTILS_CUSTOM_RW
template<typename T, typename N>
inline T Read(N addr)
{
	return *(T*)addr;
}

template<typename T, typename N>
inline void ReadArr(N addr, T* arr, size_t count)
{
	memcpy((void*)arr, *(void**)addr, sizeof(T) * count);
}

template<typename T, typename N>
inline void Write(N addr, T value)
{
	return *(T*)addr = value;
}

template<typename T, typename N>
inline void WriteArr(N addr, T* arr, size_t count)
{
	memcpy((void*)addr, (void*)arr, sizeof(T) * count);
}

#else
template<typename T, typename N>
T Read(N addr);

template<typename T, typename N>
void ReadArr(N addr, T* arr, size_t count);

template<typename T, typename N>
T Write(N addr, T value);

template<typename T, typename N>
void WriteArr(N addr, T* arr, size_t count);
#endif

inline uintptr_t GetAbsoluteAddress(uintptr_t addr, intptr_t offset, intptr_t instructionSize)
{
	return addr + Read<int>(addr + offset) + instructionSize;
}

template<typename T, size_t idx, typename N>
inline T GetVFunc(N* inst)
{
	return Read<T>(Read<T*>(inst) + idx);
}

#endif
