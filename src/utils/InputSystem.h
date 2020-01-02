#pragma once

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <chrono>
#include <thread>
#include <errno.h>

#include "../globals.h"

namespace InputSystem {
    extern void *InputSystem(void *);
}