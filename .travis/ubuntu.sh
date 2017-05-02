#!/bin/bash
sudo apt-get update -qq
sudo apt-get install -qq curl p7zip-full git
sudo apt-get -qq build-dep qt5-default
bash .travis/qt5-ubuntu.sh $(pwd) extra-charts 3d base declarative graphicaleffects imageformats multimedia quickcontrols quickcontrols2 sensors svg tools translations xmlpatterns
export QTDIR=$(pwd)/5.7/gcc_64/
export PATH=$QTDIR/bin:$PATH
qmake
make
cp -r $QTDIR/lib .snapcraft/
cp -r $QTDIR/plugins .snapcraft/
cp -r $QTDIR/qml .snapcraft/
cp neuronify .snapcraft/
