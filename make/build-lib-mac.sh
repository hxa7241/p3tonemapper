#!/bin/bash


# set constants ----------------------------------------------------------------
COMPILER=g++
LINKER=g++
COMPILE_OPTIONS="-c -fPIC -x c++ -ansi -std=c++98 -pedantic -fno-enforce-eh-specs -fno-rtti -O3 -ffast-math -fno-threadsafe-statics -ftree-vectorize -mfpmath=sse -msse -Wall -Wextra -Wabi -Wold-style-cast -Wsign-promo -Woverloaded-virtual -Wstrict-null-sentinel -Wcast-align -Wwrite-strings -Wpointer-arith -Wcast-qual -Wconversion -Wredundant-decls -Wdisabled-optimization -D _PLATFORM_LINUX -Ilibrary/src -Ilibrary/src/general -Ilibrary/src/graphics -Ilibrary/src/image -Ilibrary/src/tonemap"
LINK_OPTIONS="-shared -Wl,-soname,libp3tonemapper.so.1 -o libp3tonemapper.so.1.2"


mkdir library/obj
rm library/obj/*


# complile ---------------------------------------------------------------------
echo
$COMPILER --version
echo "--- compile --"

$COMPILER $COMPILE_OPTIONS library/src/general/Array.cpp -o library/obj/Array.o
$COMPILER $COMPILE_OPTIONS library/src/general/Clamps.cpp -o library/obj/Clamps.o
$COMPILER $COMPILE_OPTIONS library/src/general/FpToInt.cpp -o library/obj/FpToInt.o
$COMPILER $COMPILE_OPTIONS library/src/general/Histogram.cpp -o library/obj/Histogram.o
$COMPILER $COMPILE_OPTIONS library/src/general/Interval.cpp -o library/obj/Interval.o
$COMPILER $COMPILE_OPTIONS library/src/general/SamplesRegular1.cpp -o library/obj/SamplesRegular1.o
$COMPILER $COMPILE_OPTIONS library/src/general/Sheet.cpp -o library/obj/Sheet.o

$COMPILER $COMPILE_OPTIONS library/src/graphics/ColorConstants.cpp -o library/obj/ColorConstants.o
$COMPILER $COMPILE_OPTIONS library/src/graphics/Matrix3f.cpp -o library/obj/Matrix3f.o
$COMPILER $COMPILE_OPTIONS library/src/graphics/Vector3f.cpp -o library/obj/Vector3f.o

$COMPILER $COMPILE_OPTIONS library/src/image/ColorSpace.cpp -o library/obj/ColorSpace.o
$COMPILER $COMPILE_OPTIONS library/src/image/ImageRgbFloat.cpp -o library/obj/ImageRgbFloat.o
$COMPILER $COMPILE_OPTIONS library/src/image/ImageRgbFloatIter.cpp -o library/obj/ImageRgbFloatIter.o
$COMPILER $COMPILE_OPTIONS library/src/image/ImageRgbInt.cpp -o library/obj/ImageRgbInt.o

$COMPILER $COMPILE_OPTIONS library/src/tonemap/AcuityFilter.cpp -o library/obj/AcuityFilter.o
$COMPILER $COMPILE_OPTIONS library/src/tonemap/ColorAdjustment.cpp -o library/obj/ColorAdjustment.o
$COMPILER $COMPILE_OPTIONS library/src/tonemap/Foveal.cpp -o library/obj/Foveal.o
$COMPILER $COMPILE_OPTIONS library/src/tonemap/PerceptualMap.cpp -o library/obj/PerceptualMap.o
$COMPILER $COMPILE_OPTIONS library/src/tonemap/ToneAdjustment.cpp -o library/obj/ToneAdjustment.o
$COMPILER $COMPILE_OPTIONS library/src/tonemap/Veil.cpp -o library/obj/Veil.o

$COMPILER $COMPILE_OPTIONS library/src/p3tmPerceptualMap.cpp -o library/obj/p3tmPerceptualMap.o


# link -------------------------------------------------------------------------
echo
echo "--- link --"

$LINKER $LINK_OPTIONS library/obj/*.o


##mv libp3tonemapper.so.1.2 /usr/lib
ln -sf libp3tonemapper.so.1.2 libp3tonemapper.so.1
##ldconfig -n .
ln -sf libp3tonemapper.so.1 libp3tonemapper.so


rm library/obj/*


echo
echo "--- done --"


exit
