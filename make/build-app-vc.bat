@echo off


rem --- using: MS VC++ 2005 ---


set COMPILER=cl
set LINKER=link
set COMPILE_OPTIONS=/c /O2 /GL /fp:fast /EHsc /GR- /GS- /MT /W4 /WL /nologo /D_PLATFORM_WIN /Iapplication/src /Iapplication/src/general /Iapplication/src/format



mkdir application\obj
del /Q application\obj\*



@echo.
@echo --- compile --

%COMPILER% %COMPILE_OPTIONS% application/src/general/DynamicLibraryInterface.cpp /Foapplication/obj/DynamicLibraryInterface.obj
%COMPILER% %COMPILE_OPTIONS% application/src/general/StringConstants.cpp /Foapplication/obj/StringConstants.obj

%COMPILER% %COMPILE_OPTIONS% application/src/format/exr.cpp /Foapplication/obj/exr.obj
%COMPILER% %COMPILE_OPTIONS% application/src/format/png.cpp /Foapplication/obj/png.obj
%COMPILER% %COMPILE_OPTIONS% application/src/format/ppm.cpp /Foapplication/obj/ppm.obj
%COMPILER% %COMPILE_OPTIONS% application/src/format/rgbe.cpp /Foapplication/obj/rgbe.obj
%COMPILER% %COMPILE_OPTIONS% application/src/format/ImageFormatter.cpp /Foapplication/obj/ImageFormatter.obj
%COMPILER% %COMPILE_OPTIONS% application/src/format/ImageRef.cpp /Foapplication/obj/ImageRef.obj

%COMPILER% %COMPILE_OPTIONS% application/src/p3tonemapper.cpp /Foapplication/obj/p3tonemapper.obj



@echo.
@echo --- link --

%LINKER% /LTCG /OPT:REF /OPT:NOWIN98 /VERSION:1.3 /NOLOGO /OUT:p3tonemapper.exe kernel32.lib advapi32.lib p3tonemapper.lib application/obj/*.obj


del /Q application\obj\*
