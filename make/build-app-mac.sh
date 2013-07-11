#!/bin/bash


# set constants ----------------------------------------------------------------
COMPILER=g++
LINKER=g++
COMPILE_OPTIONS="-c -x c++ -ansi -std=c++98 -pedantic -fno-enforce-eh-specs -fno-rtti -O3 -ffast-math -fno-threadsafe-statics -ftree-vectorize -mfpmath=sse -msse -Wall -Wextra -Wabi -Wold-style-cast -Wsign-promo -Woverloaded-virtual -Wstrict-null-sentinel -Wcast-align -Wwrite-strings -Wpointer-arith -Wcast-qual -Wconversion -Wredundant-decls -Wdisabled-optimization -D _PLATFORM_LINUX -Iapplication/src -Iapplication/src/general -Iapplication/src/image -Iapplication/src/format"


mkdir application/obj
rm application/obj/*


# compile ----------------------------------------------------------------------
echo
$COMPILER --version
echo "--- compile --"

$COMPILER $COMPILE_OPTIONS application/src/general/DynamicLibraryInterface.cpp -o application/obj/DynamicLibraryInterface.o
$COMPILER $COMPILE_OPTIONS application/src/general/StringConstants.cpp -o application/obj/StringConstants.o

$COMPILER $COMPILE_OPTIONS application/src/format/exr.cpp -o application/obj/exr.o
$COMPILER $COMPILE_OPTIONS application/src/format/png.cpp -o application/obj/png.o
$COMPILER $COMPILE_OPTIONS application/src/format/ppm.cpp -o application/obj/ppm.o
$COMPILER $COMPILE_OPTIONS application/src/format/rgbe.cpp -o application/obj/rgbe.o
$COMPILER $COMPILE_OPTIONS application/src/format/ImageFormatter.cpp -o application/obj/ImageFormatter.o
$COMPILER $COMPILE_OPTIONS application/src/format/ImageRef.cpp -o application/obj/ImageRef.o

$COMPILER $COMPILE_OPTIONS application/src/p3tonemapper.cpp -o application/obj/p3tonemapper.o


# link -------------------------------------------------------------------------
echo
echo "--- link --"

$LINKER -Wl,-rpath,. -o p3tonemapper application/obj/*.o -L. -lp3tonemapper -ldl


rm application/obj/*


echo
echo "--- done --"


exit
