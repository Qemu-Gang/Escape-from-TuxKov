#pragma once

extern WinProcess* process;

template<typename T, size_t off>
struct OffPtr;

class ProcessBaseClass
{
  protected:
    template<typename T, size_t off>
    friend struct OffPtr;

    char* readBuf;
  public:
    uintptr_t address;

    ProcessBaseClass(char* rBuf, uintptr_t addr = 0)
        : readBuf(rBuf), address(addr) {}
};

template<typename T, size_t off>
struct OffPtr
{
    const ProcessBaseClass& baseClass;
    bool dirty = false;

    OffPtr(const ProcessBaseClass& base)
        : baseClass(base) { }

    OffPtr(const ProcessBaseClass& base, size_t& maxOff)
        : OffPtr(base)
    {
        maxOff = maxOff > off ? maxOff : off;
    }

    constexpr T& GetDirect() const
    {
        return *(T*)((uintptr_t)baseClass.readBuf + off);
    }

    constexpr operator T() const
    {
        return GetDirect();
    }

    inline auto& operator=(const T& v)
    {
        GetDirect() = v;
        dirty = true;
        return *this;
    }

    inline void WriteBack(WriteList& writeList)
    {
        if (dirty)
            writeList.Write(baseClass.address + off, GetDirect());
        dirty = false;
    }
};
