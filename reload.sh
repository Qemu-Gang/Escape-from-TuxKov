#!/usr/bin/env bash

qemu_pid=$(pidof qemu-system-x86_64)
filename="$(pwd)/build/libApe-ex.so"


if grep -q "$filename" /proc/"$qemu_pid"/maps; then
	kill -24 "$qemu_pid"
	sleep 1
    sudo gdb -n -q -batch \
  	-ex "set logging on" \
  	-ex "set logging file /dev/null" \
  	-ex "set logging redirect on" \
    -ex "attach $qemu_pid" \
    -ex "set \$dlopen = (void*(*)(char*, int)) dlopen" \
    -ex "set \$dlclose = (int(*)(void*)) dlclose" \
    -ex "set \$library = \$dlopen(\"$filename\", 1)" \
    -ex "call \$dlclose(\$library)" \
	-ex "call \$dlclose(\$library)" \
    -ex "detach" \
    -ex "quit"
else
	echo "Ape-ex is not injected!"
fi

if grep -q "$filename" /proc/"$qemu_pid"/maps; then
    /bin/echo -e "\\e[33mApe-ex is already injected... Aborting...\\e[0m"
    exit
fi

sudo gdb -n -q -batch \
  	-ex "set logging on" \
  	-ex "set logging file /dev/null" \
  	-ex "set logging redirect on" \
    -ex "attach $qemu_pid" \
    -ex "set \$dlopen = (void*(*)(char*, int)) dlopen" \
    -ex "set \$library = \$dlopen(\"$filename\", 1)" \
    -ex "detach" \
    -ex "quit"