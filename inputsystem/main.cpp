// State of the Art Windows Input grabber
// build with c++11

#include <iostream>
#include <windows.h>
#include <chrono>
#include <thread>

volatile static int keys = 0;
#define KEY_ALT (1 << 0)
#define KEY_MOUSE4 (1 << 1)
#define KEY_MOUSE5 (1 << 2)

int main()
{
    std::cout << "Keys @+" << std::hex << (uintptr_t)&keys - (uintptr_t)GetModuleHandle(NULL) << std::endl;

    while (true) {
        if (GetAsyncKeyState(VK_LMENU) & 0x8000) {
            keys |= KEY_ALT;
        }
        else {
            keys &= ~KEY_ALT;
        }
        if (GetAsyncKeyState(VK_XBUTTON1) & 0x8000) {
            keys |= KEY_MOUSE4;
        }
        else {
            keys &= ~KEY_MOUSE4;
        }
        if (GetAsyncKeyState(VK_XBUTTON2) & 0x8000) {
            keys |= KEY_MOUSE5;
        }
        else {
            keys &= ~KEY_MOUSE5;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}