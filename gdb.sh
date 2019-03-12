#!/bin/bash
# Starts a GDB session on Qemu,
# Sets a couple of things up and then you can use GDB normally

qemu_pid=$(pidof qemu-system-x86_64)
if [ -z "$qemu_pid" ]; then
    /bin/echo -e "\e[31mQemu needs to be open...\e[0m"
    exit 1
fi

#https://www.kernel.org/doc/Documentation/security/Yama.txt
sudo echo "2" | sudo tee /proc/sys/kernel/yama/ptrace_scope # Only allows root to inject code. This is temp until reboot.

echo "Qemu PID: " $qemu_pid

(sudo echo -e "set \$dlopen = (void*(*)(char*, int)) dlopen\n" \
"set \$dlmopen = (void* (*)(long int, char*, int)) dlmopen\n" \
"set \$dlinfo = (int (*)(void*, int, void*)) dlinfo\n" \
"set \$malloc = (void* (*)(long long)) malloc\n" \
"set \$dlerror = (char* (*)(void)) dlerror\n" \
""; cat) | sudo gdb -p $qemu_pid