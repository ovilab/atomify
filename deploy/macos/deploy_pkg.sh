#!/bin/bash
# http://thegreyblog.blogspot.no/2014/06/os-x-creating-packages-from-command_2.html
# http://blog.qt.io/blog/2012/04/03/how-to-publish-qt-applications-in-the-mac-app-store-2/
# http://wiki.phisys.com/index.php/How-To_Qt5.3_Mac_AppStore#Entitlements
# Create icons with
# iconutil -c icns icon.iconset
# sudo installer -store -pkg Atomify.pkg -target /
# https://github.com/electron-userland/electron-builder/issues/890
# https://developer.apple.com/library/content/documentation/IDEs/Conceptual/AppDistributionGuide/MaintainingCertificates/MaintainingCertificates.html#//apple_ref/doc/uid/TP40012582-CH31-SW41
# spctl --assess --type execute --verbose --ignore-cache --no-cache

# Assumes that lammps is built with correct macos version
# Assumes that clang-omp is installed with `brew install llvm` and symlinked as clang-omp++
# Assumes that libomp and libstd etc are code signed before linking

# If qmake is not in path, specify it here
export PATH=$PATH:/Users/anderhaf/Qt/5.10.1/clang_64/bin/

# Verify that 10.9 SDK exists
if [ ! -d "/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.9.sdk" ]; then
  echo "Could not find 10.9 SDK at /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.9.sdk"
  echo "Please download SDK from https://github.com/phracker/MacOSX-SDKs"
  exit 1
fi

mkdir build-pkg
cd build-pkg
qmake ../../../src DEPLOYPKG=true
# Force using 10.9 SDK even if current OS is newer. Not sure how to fix this in qmake call
sed -i "" 's/MacOSX10.10.sdk/MacOSX10.9.sdk/g' Makefile
sed -i "" 's/MacOSX10.11.sdk/MacOSX10.9.sdk/g' Makefile
sed -i "" 's/MacOSX10.12.sdk/MacOSX10.9.sdk/g' Makefile
sed -i "" 's/MacOSX10.13.sdk/MacOSX10.9.sdk/g' Makefile
make -j8

# First create folder to create dmg
mkdir Atomify
cp -r Atomify.app Atomify
cd Atomify
ln -s /Applications Applications
xattr -cr Atomify.app
macdeployqt Atomify.app -qmldir=../../../../src/qml -codesign="Developer ID Application: Anders Hafreager (4XKET6P69R)" -appstore-compliant
cd ..


# Create dmg with shortcut to Applications
mkdir dmg
cd dmg
hdiutil create Atomify.dmg -volname "Atomify" -srcfolder ../Atomify/
cp Atomify.dmg ../../Atomify-v2.2-macos.dmg
cd ..

# Now create folder for pkg
mkdir pkg
rm -rf pkg/*
cp -r Atomify.app pkg
cd pkg
xattr -cr Atomify.app
macdeployqt Atomify.app -dmg -qmldir=../../../../src/qml -codesign="Developer ID Application: Anders Hafreager (4XKET6P69R)" -appstore-compliant
cd "Atomify.app"
find . -name *.dSYM | xargs -I $ rm -R $
cd ..
productbuild --component Atomify.app /Applications --sign "Developer ID Installer: Anders Hafreager" Atomify-v2.2-macos-installer.pkg
cp Atomify-v2.2-macos-installer.pkg ../../
