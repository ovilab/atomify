TEMPLATE = app
CONFIG += c++14
CONFIG += appstore
DEFINES += LAMMPS_GZIP
TARGET = atomify

QT += qml quick widgets opengl openglextensions svg charts datavisualization
DEFINES += ATOMIFYVERSION=\\\"2.2a\\\"
unix:!macx {
    QMAKE_CXXFLAGS += -fopenmp
    LIBS += -fopenmp
}

defined(DEPLOYLINUX, var) {
    QMAKE_LFLAGS += -static-libstdc++
    DEFINES += BUILDVERSION=\\\"LINUX\\\"
    BUILDVERSION = true
}

defined(DEPLOYMACAPPSTORE, var) {
    DEFINES += MACAPPSTORE
    DEFINES += BUILDVERSION=\\\"MACAPPSTORE\\\"
    BUILDVERSION = true
}

defined(DEPLOYPKG, var) {
    QMAKE_CXX  = /usr/local/bin/clang-omp++
    QMAKE_LINK = /usr/local/bin/clang-omp++
    QMAKE_CXXFLAGS += -fopenmp
    LIBS += -fopenmp
    QMAKE_LFLAGS += -L/usr/local/opt/llvm/lib -Wl,-rpath,/usr/local/opt/llvm/lib
    QMAKE_CFLAGS += -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.9.sdk
    QMAKE_CXXFLAGS += -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.9.sdk
    QMAKE_LFLAGS += -Wl,-syslibroot,/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.9.sdk

    QMAKE_MAC_SDK = macosx10.9
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.9
    DEFINES += BUILDVERSION=\\\"MACPKG\\\"
    BUILDVERSION = true
}

!defined(BUILDVERSION, var) {
    DEFINES += BUILDVERSION=\\\"BUILD\\\"
}

macx {
    ICON = images/icon.icns
    QMAKE_INFO_PLIST = ../macos/macos.plist
    TARGET = Atomify
}

ios {
    QMAKE_INFO_PLIST = ../ios/ios.plist
}

CONFIG += warn_off
DEFINES += LAMMPS_EXCEPTIONS LAMMPS_GZIP LAMMPS_MEMALIGN=64
# lammps.pri includes the path and libs to lammps
# Run configure.py to generate lammps.pri

android {
    DEFINES += LAMMPS_XDR
    SOURCES +=
    HEADERS +=
    include(../lammps-android.pri)
} else:ios {
    include(../lammps-ios.pri)
} else {
    include(../lammps.pri)
}
RESOURCES += images.qrc \
    qml/qml.qrc \
    help.qrc \
    extras.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

!isEmpty(PREFIX) {
    target.path=$$PREFIX/
    export(target.path)
}

# Default rules for deployment.
include(deployment.pri)
#include(../SimVis/examples/examples_vendor.pri) # replace with qpm installation
include(../libs/SimVis/package_vendor.pri)
include(../libs/QmlPreviewer/qmlpreviewer.pri)

!exists(examples/examples.qrc) {
    error("examples/examples.qrc missing. Did you forget to run python configure.py?")
}
CONFIG += resources_big
RESOURCES += examples/examples.qrc

SOURCES += \
    main.cpp \
    lammpscontroller.cpp \
    highlighter.cpp \
    datasource.cpp \
    LammpsWrappers/atomdata.cpp \
    dataproviders/dataprovider.cpp \
    codeeditorbackend.cpp \
    linenumbers.cpp \
    dataproviders/data2d.cpp \
    windowgl2.cpp \
    mousemover.cpp \
    states.cpp \
    dataproviders/data1d.cpp \
    parsefileuploader.cpp \
    standardpaths.cpp \
    keysequence.cpp \
    usagestatistics.cpp \
    registerqml.cpp \
    helpers/clipboard.cpp \
    rendercapturehelper.cpp \
    lammpsaspect.cpp \
    lammpsthread.cpp \
    tmplammps.cpp \
    backendlammpscontroller.cpp

HEADERS += \
    lammpscontroller.h \
    highlighter.h \
    datasource.h \
    LammpsWrappers/atomdata.h \
    dataproviders/dataprovider.h \
    codeeditorbackend.h \
    linenumbers.h \
    dataproviders/data2d.h \
    windowgl2.h \
    mousemover.h \
    states.h \
    dataproviders/data1d.h \
    parsefileuploader.h \
    standardpaths.h \
    keysequence.h \
    usagestatistics.h \
    helpers/clipboard.h \
    rendercapturehelper.h \
    lammpsaspect.h \
    lammpsthread.h \
    tmplammps.h \
    backendlammpscontroller.h

# Temporary use of quickcontrols2 without install

RC_ICONS = atomify_lammps/images/atomify_logo.ico

DISTFILES += \
    ../ios/ios.plist \
    ../macos/macos.plist \
    qml/qtquickcontrols2.conf \
    ../.travis.yml \
    ../snappy-qt5.conf \
    ../atomify-lammps.wrapper \
    ../snapcraft.yaml \
    ../configure.py \
    qml/qmldir \
    qml/events/qmldir \
    qml/events/EventCenter.qml \
    qml/events/Shortcut.qml \
    qml/events/EventMapper.qml

STATECHARTS +=
