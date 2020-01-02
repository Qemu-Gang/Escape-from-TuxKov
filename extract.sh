#!/usr/bin/env bash

qemu_pid=$(pidof qemu-system-x86_64)
filename="$(pwd)/build/libHalo-ReachIn.so"

if grep -q "$filename" /proc/"$qemu_pid"/maps; then
    sudo gdb -n -q -batch \
  	-ex "set logging on" \
  	-ex "set logging file /dev/null" \
  	-ex "set logging redirect on" \
    -ex "attach $qemu_pid" \
    -ex "set \$dlopen = (void*(*)(char*, int)) dlopen" \
    -ex "set \$dlclose = (int(*)(void*)) dlclose" \
    -ex "set \$library = \$dlopen(\"$filename\", 6)" \
    -ex "call \$dlclose(\$library)" \
	-ex "call \$dlclose(\$library)" \
    -ex "quit"
else
	echo "Library is not injected!"
fi

