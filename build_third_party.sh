#!/bin/bash
#set -x
git submodule init
git submodule update

if [ ! -f /proc/cpuinfo ]; then
	echo "Is this Linux? Cannot find or read /proc/cpuinfo"
	exit 1
fi
NUMPROC=$(grep 'processor' /proc/cpuinfo | wc -l)

cd third_party/escargot/
./build_third_party.sh
make clean
make x64.interpreter.release.static -j8
make x64.interpreter.debug.static -j8

if [ -n "$TIZEN_SDK_HOME" ]; then
make tizen_wearable_arm.interpreter.debug.static -j8
make tizen_wearable_arm.interpreter.release.static -j8
make tizen_wearable_emulator.interpreter.debug.static -j8
make tizen_wearable_emulator.interpreter.release.static -j8
fi
if [ -n "$TIZEN_SDK_HOME" ]; then
make tizen3_wearable_arm.interpreter.debug.static -j8
make tizen3_wearable_arm.interpreter.release.static -j8
make tizen3_wearable_emulator.interpreter.debug.static -j8
make tizen3_wearable_emulator.interpreter.release.static -j8
fi

