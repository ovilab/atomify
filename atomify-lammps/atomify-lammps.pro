TEMPLATE = app
CONFIG += c++11
DEFINES += LAMMPS_GZIP

QT += qml quick widgets opengl openglextensions svg charts
QMAKE_CXXFLAGS += -Wno-unused-parameter -Wno-unused-variable
#QMAKE_CXXFLAGS += -g -fopenmp
#LIBS += -fopenmp
CONFIG += warn_off

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
    simulations/diffusion/diffusion.qrc \
    simulations/other/other.qrc \
    simulations/water/water.qrc \
    simulations/potentialparameters/potentialparameters.qrc \
    simulations/silica/silica.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)
include(../SimVis/package_vendor.pri)

ios {
    QMAKE_INFO_PLIST = iOS.plist
}

SOURCES += \
    main.cpp \
    mysimulator.cpp \
    lammpscontroller.cpp \
    highlighter.cpp \
    scriptparser.cpp \
    scripthandler.cpp \
    scriptcommand.cpp \
    LammpsWrappers/nvt.cpp \
    LammpsWrappers/simulatorcontrol.cpp \
    LammpsWrappers/cpfix.cpp \
    LammpsWrappers/cpcompute.cpp \
    LammpsWrappers/cpvariable.cpp \
    LammpsWrappers/fixaveragetime.cpp \
    LammpsWrappers/fixhistogram.cpp \
    LammpsWrappers/rdf.cpp \
    datasource.cpp \
    LammpsWrappers/system.cpp \
    LammpsWrappers/atoms.cpp \
    LammpsWrappers/neighborlist.cpp \
    LammpsWrappers/lammpserror.cpp \
    LammpsWrappers/atomdata.cpp \
    LammpsWrappers/modifiers/colormodifier.cpp \
    LammpsWrappers/modifiers/modifier.cpp \
    LammpsWrappers/modifiers/periodicimages.cpp \
    LammpsWrappers/cpbonds.cpp

HEADERS += \
    mysimulator.h \
    lammpscontroller.h \
    highlighter.h \
    scriptparser.h \
    scripthandler.h \
    scriptcommand.h \
    LammpsWrappers/nvt.h \
    LammpsWrappers/simulatorcontrol.h \
    LammpsWrappers/cpfix.h \
    LammpsWrappers/cpcompute.h \
    LammpsWrappers/cpvariable.h \
    LammpsWrappers/fixaveragetime.h \
    LammpsWrappers/fixhistogram.h \
    LammpsWrappers/rdf.h \
    datasource.h \
    LammpsWrappers/system.h \
    LammpsWrappers/atoms.h \
    LammpsWrappers/neighborlist.h \
    LammpsWrappers/lammpserror.h \
    LammpsWrappers/atomdata.h \
    LammpsWrappers/modifiers/colormodifier.h \
    LammpsWrappers/modifiers/modifier.h \
    LammpsWrappers/modifiers/modifiers.h \
    LammpsWrappers/modifiers/periodicimages.h \
    LammpsWrappers/cpbonds.h

# Temporary use of quickcontrols2 without install


DISTFILES += \
    iOS.plist \
    simulations/diffusion/indent/indent.png \
    simulations/diffusion/indent/indent.in \
    simulations/diffusion/indent/indent.qml \
    android/AndroidManifest.xml \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradlew \
    android/res/values/libs.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew.bat

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
