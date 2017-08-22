#!/bin/bash

if [ ! -f ./linuxdeployqt-continuous-x86_64.AppImage ]; then
	wget https://github.com/probonopd/linuxdeployqt/releases/download/continuous/linuxdeployqt-continuous-x86_64.AppImage 
	chmod a+x ./linuxdeployqt-continuous-x86_64.AppImage
fi

mkdir build
cd build
qmake ../../src DEPLOYLINUX=true
make -j4

mkdir -p deploy/atomify
rm -rf deploy/atomify/*
cp -r atomify deploy/atomify/
cd deploy/atomify/

# Must do twice due to a bug (reported on github)
../../../linuxdeployqt-continuous-x86_64.AppImage ./atomify -qmldir=../../../../src/qml/
../../../linuxdeployqt-continuous-x86_64.AppImage ./atomify -qmldir=../../../../src/qml/
cp -r ~/Qt/5.9.1/gcc_64/qml/QtQuick/Controls/* qml/QtQuick/Controls/

cd ..
tar -cvzf ../../atomify-2.1.0-linux-x86_64.tar.gz atomify
