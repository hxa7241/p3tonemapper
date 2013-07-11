@echo off

c:\h\tools\zip -d p3tonemapper13exe-win.zip *

mkdir dev
copy p3tonemapper.lib dev
copy library\src\p3tmPerceptualMap.h dev
copy library\src\p3tmPerceptualMap-v13.h dev

cd ..

c:\h\tools\zip -9ro p3tonemapper/p3tonemapper13exe-win.zip p3tonemapper -x p3tonemapper/docs/* -x p3tonemapper/images/* -x p3tonemapper/make/* -x p3tonemapper/temp/* -x p3tonemapper/application/* -x p3tonemapper/library/* -x *.bat -x *.exp -x p3tonemapper/p3tonemapper.lib

c:\h\tools\zip -T p3tonemapper/p3tonemapper13exe-win.zip

cd p3tonemapper
