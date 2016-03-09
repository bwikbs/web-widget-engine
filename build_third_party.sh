#!/bin/bash

git submodule init
git submodule update

if [ ! -f /proc/cpuinfo ]; then
	echo "Is this Linux? Cannot find or read /proc/cpuinfo"
	exit 1
fi
NUMPROC=$(grep 'processor' /proc/cpuinfo | wc -l)

###########################################################
# GC build
###########################################################
#cd third_party/bdwgc/
#autoreconf -vif
#automake --add-missing

#rm -rf out

#mkdir -p out/x86/release
#mkdir -p out/x86/debug
#mkdir -p out/x64/release
#mkdir -p out/x64/debug

#GCCONFFLAGS=" --disable-parallel-mark " # --enable-large-config --enable-cplusplus"

#cd out/x86/release
#../../../configure $GCCONFFLAGS --disable-gc-debug CFLAGS='-m32' CXXFLAGS='-m32' LDFLAGS='-m32'
#make -j$NUMPROC
#cd -

#cd out/x86/debug
#../../../configure $GCCONFFLAGS CFLAGS='-m32 -g3' CXXFLAGS='-m32' LDFLAGS='-m32' --enable-debug
#make -j$NUMPROC
#cd -

#cd out/x64/release
#../../../configure $GCCONFFLAGS --disable-gc-debug CFLAGS='' CXXFLAGS='' LDFLAGS=''
#make -j$NUMPROC
#cd -

#cd out/x64/debug
#../../../configure $GCCONFFLAGS CFLAGS='-g3' CXXFLAGS='' LDFLAGS='' --enable-debug
#make -j$NUMPROC
#cd -

#cd ..
#cd ..

cd third_party/escargot/
./build_third_party.sh && make x64.interpreter.release.shared -j && make x64.interpreter.debug.shared -j

if [ -n "$TIZEN_SDK_HOME" ]; then
make tizen_wearable_arm.interpreter.release.shared -j8
make tizen_wearable_emulator.interpreter.release.shared -j8
fi

