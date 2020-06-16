#include "InputSystem.h"
#include "Logger.h"

// https://github.com/LWSS/evdev-mirror

// This struct is the same at <linux/input.h>
struct input_value {
    uint16_t type;
    uint16_t code;
    int32_t value;
};

void *InputSystem::InputSystem(void *) {
    int fd;
    struct input_value input;

    fd = open("/dev/input/evdev-mirror", O_RDONLY | O_NONBLOCK);
    if (fd < 0) {
        Logger::Log("Error opening evdev-mirror! (%d), %d\nInput will be disabled.\n", fd, errno);
        return nullptr;
    }
    Logger::Log("Started input loop\n");
    while (running) {
        // if zero bytes, keep goin...
        if (!read(fd, &input, sizeof(struct input_value))) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            continue;
        }
        if( input.type != EV_KEY ){
            continue;
        }
        pressedKeys[input.code] = input.value > 0;
        //Logger::Log("Key: %d - state: %d\n", input.code, input.value);
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    return nullptr;
}