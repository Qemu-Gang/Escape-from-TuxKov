// State of the Art Windows Input grabber
// build with c++11

#include <iostream>
#include <windows.h>
#include <chrono>
#include <thread>

static volatile bool mouse3;
static volatile bool mouse4;

int main()
{
    std::cout<<"Mouse3 @+"<< std::hex << (uintptr_t)&mouse3 - (uintptr_t)GetModuleHandle(NULL) << std::endl;
    std::cout<<"Mouse4 @+"<< std::hex << (uintptr_t)&mouse4 - (uintptr_t)GetModuleHandle(NULL) << std::endl;
    while( true ){
        mouse4 = GetKeyState(0x05) & 0x80;
        mouse3 = GetKeyState(0x06) & 0x80;
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    return 0;
}