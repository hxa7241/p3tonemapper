#!/bin/bash

mkdir dev
cp application/src/p3tmPerceptualMap.h dev
cp application/src/p3tmPerceptualMap-v13.h dev

cd ..

tar --exclude=p3tonemapper/release* --exclude=p3tonemapper/application --exclude=p3tonemapper/library --exclude=p3tonemapper/docs --exclude=p3tonemapper/make -cf p3tonemapper13exe-linux.tar p3tonemapper

gzip -q -9 p3tonemapper13exe-linux.tar

cd p3tonemapper

exit
