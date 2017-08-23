#!/bin/bash
# http://thegreyblog.blogspot.no/2014/06/os-x-creating-packages-from-command_2.html
# http://blog.qt.io/blog/2012/04/03/how-to-publish-qt-applications-in-the-mac-app-store-2/
# http://wiki.phisys.com/index.php/How-To_Qt5.3_Mac_AppStore#Entitlements
# Create icons with
# iconutil -c icns icon.iconset
# sudo installer -store -pkg Atomify.pkg -target /

# Assumes that lammps is built with correct macos version
# Assumes that clang-omp is installed with `brew install llvm` and symlinked as clang-omp++
# Assumes that libomp and libstd etc are code signed before linking

mkdir build-pkg
cd build-pkg
qmake ../../../src DEPLOYPKG=true
make -j8

mkdir pkg
rm -rf pkg/*
cp -r Atomify.app pkg
cd pkg
xattr -cr Atomify.app
macdeployqt Atomify.app -dmg -qmldir=../../../../src/qml -codesign="3rd Party Mac Developer Application: Anders Hafreager" -appstore-compliant
cd "Atomify.app"
find . -name *.dSYM | xargs -I $ rm -R $
cd ..
productbuild --component Atomify.app /Applications --sign "Developer ID Installer: Anders Hafreager" Atomify-2.1.0-macos.pkg
cp Atomify-2.1.0-macos.pkg ../../
