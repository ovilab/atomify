#!/bin/bash
# http://thegreyblog.blogspot.no/2014/06/os-x-creating-packages-from-command_2.html
# http://blog.qt.io/blog/2012/04/03/how-to-publish-qt-applications-in-the-mac-app-store-2/
# http://wiki.phisys.com/index.php/How-To_Qt5.3_Mac_AppStore#Entitlements
# Create icons with
# iconutil -c icns icon.iconset
# sudo installer -store -pkg Atomify.pkg -target /
rm -r Atomify.app
cp -r atomify-lammps.app Atomify.app
xattr -cr Atomify.app
~/Qt/5.9/clang_64/bin/macdeployqt Atomify.app -qmldir=/projects/atomify/editor/atomify-lammps/atomify-lammps/qml -codesign="3rd Party Mac Developer Application: Anders Hafreager" -appstore-compliant
cd "Atomify.app"
find . -name *.dSYM | xargs -I $ rm -R $
cd ..
cp atomify-lammps.app/Contents/MacOS/atomify-lammps Atomify.app/Contents/MacOS/
codesign -s "3rd Party Mac Developer Application: Anders Hafreager" --entitlements /projects/atomify/editor/atomify-lammps/atomify-lammps/entitlements.plist Atomify.app
productbuild --component Atomify.app /Applications --sign "3rd Party Mac Developer Installer: Anders Hafreager" Atomify.pkg
~/Qt/5.9/clang_64/bin/macdeployqt Atomify.app -qmldir=/projects/atomify/editor/atomify-lammps/atomify-lammps/qml -codesign="3rd Party Mac Developer Application: Anders Hafreager" -appstore-compliant
productbuild --component Atomify.app /Applications --sign "3rd Party Mac Developer Installer: Anders Hafreager" Atomify.pkg