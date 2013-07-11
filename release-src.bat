@echo off

cd ..

c:\h\tools\zip -d   p3tonemapper/p3tonemapper13src.zip *

c:\h\tools\zip -9ro p3tonemapper/p3tonemapper13src.zip p3tonemapper -x p3tonemapper/temp/* -x p3tonemapper/images/* -x p3tonemapper/application/lib/* -x p3tonemapper/application/obj/* -x p3tonemapper/application/temp/* -x p3tonemapper/application/p3tonemapper-opt.txt -x p3tonemapper/library/obj/* -x */*.exe -x */*.dll -x */*.lib -x */*.a -x */*.exp -x */*.zip -x */*.gz -x */*.log -x */*.data -x */*.tws -x */*.lnk -x */release-* -x */zip-p3tm.bat -x */p3-tonemapper-construction.txt -x */p3-tonemapper-transition.txt -x */genhtmldoc.bat -x */markdown-converter.txt -x */setwinenv.bat -x */build-vc.bat -x */build-gcc.bat -x */clean.bat -x */movelib.bat -x */HxaBuild-options-vc.txt -x */HxaBuild-options-gcc.txt

c:\h\tools\zip -T   p3tonemapper/p3tonemapper13src.zip

cd p3tonemapper
