@echo off


rem --- using: MS VC++ 2005 ---


set COMPILER=cl
set LINKER=link
set COMPILE_OPTIONS=/c /O2 /GL /fp:fast /EHsc /GR- /GS- /MT /W4 /WL /nologo /D_CRT_SECURE_NO_DEPRECATE /D_PLATFORM_WIN /Ilibrary/src /Ilibrary/src/general /Ilibrary/src/graphics /Ilibrary/src/image /Ilibrary/src/tonemap



mkdir library\obj
del /Q library\obj\*



@echo.
@echo --- compile --

%COMPILER% %COMPILE_OPTIONS% library/src/general/Array.cpp /Folibrary/obj/Array.obj
%COMPILER% %COMPILE_OPTIONS% library/src/general/Clamps.cpp /Folibrary/obj/Clamps.obj
%COMPILER% %COMPILE_OPTIONS% library/src/general/FpToInt.cpp /Folibrary/obj/FpToInt.obj
%COMPILER% %COMPILE_OPTIONS% library/src/general/Histogram.cpp /Folibrary/obj/Histogram.obj
%COMPILER% %COMPILE_OPTIONS% library/src/general/Interval.cpp /Folibrary/obj/Interval.obj
%COMPILER% %COMPILE_OPTIONS% library/src/general/SamplesRegular1.cpp /Folibrary/obj/SamplesRegular1.obj
%COMPILER% %COMPILE_OPTIONS% library/src/general/Sheet.cpp /Folibrary/obj/Sheet.obj

%COMPILER% %COMPILE_OPTIONS% library/src/graphics/ColorConstants.cpp /Folibrary/obj/ColorConstants.obj
%COMPILER% %COMPILE_OPTIONS% library/src/graphics/Matrix3f.cpp /Folibrary/obj/Matrix3f.obj
%COMPILER% %COMPILE_OPTIONS% library/src/graphics/Vector3f.cpp /Folibrary/obj/Vector3f.obj

%COMPILER% %COMPILE_OPTIONS% library/src/image/ColorSpace.cpp /Folibrary/obj/ColorSpace.obj
%COMPILER% %COMPILE_OPTIONS% library/src/image/ImageRgbFloat.cpp /Folibrary/obj/ImageRgbFloat.obj
%COMPILER% %COMPILE_OPTIONS% library/src/image/ImageRgbFloatIter.cpp /Folibrary/obj/ImageRgbFloatIter.obj
%COMPILER% %COMPILE_OPTIONS% library/src/image/ImageRgbInt.cpp /Folibrary/obj/ImageRgbInt.obj

%COMPILER% %COMPILE_OPTIONS% library/src/tonemap/AcuityFilter.cpp /Folibrary/obj/AcuityFilter.obj
%COMPILER% %COMPILE_OPTIONS% library/src/tonemap/ColorAdjustment.cpp /Folibrary/obj/ColorAdjustment.obj
%COMPILER% %COMPILE_OPTIONS% library/src/tonemap/Foveal.cpp /Folibrary/obj/Foveal.obj
%COMPILER% %COMPILE_OPTIONS% library/src/tonemap/PerceptualMap.cpp /Folibrary/obj/PerceptualMap.obj
%COMPILER% %COMPILE_OPTIONS% library/src/tonemap/ToneAdjustment.cpp /Folibrary/obj/ToneAdjustment.obj
%COMPILER% %COMPILE_OPTIONS% library/src/tonemap/Veil.cpp /Folibrary/obj/Veil.obj

%COMPILER% %COMPILE_OPTIONS% library/src/p3tmPerceptualMap.cpp /Folibrary/obj/p3tmPerceptualMap.obj



@echo.
@echo --- link --

%LINKER% /DLL /DEF:library/p3tmPerceptualMap.def /LTCG /OPT:REF /OPT:NOWIN98 /VERSION:1.3 /NOLOGO /OUT:p3tonemapper.dll kernel32.lib advapi32.lib library/obj/*.obj


del /Q library\obj\*
