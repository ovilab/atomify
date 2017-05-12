TEMPLATE = app
CONFIG += c++11
CONFIG += appstore
DEFINES += LAMMPS_GZIP
TARGET = atomify

QT += qml quick widgets opengl openglextensions svg charts datavisualization

unix:!macx {
    QMAKE_CXXFLAGS += -fopenmp
    LIBS += -fopenmp
}

macx {
    QMAKE_MAC_SDK = macosx10.9
    ICON = images/icon.icns
    QMAKE_INFO_PLIST = ../macos/macos.plist
    TARGET = Atomify
}

# DEFINES += MACAPPSTORE

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
    simulations/diffusion/diffusion.qrc \
    simulations/other/other.qrc \
    simulations/water/water.qrc \
    simulations/potentialparameters/potentialparameters.qrc \
    simulations/silica/silica.qrc \
    help.qrc

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
    mysimulator.cpp \
    lammpscontroller.cpp \
    highlighter.cpp \
    datasource.cpp \
    LammpsWrappers/simulatorcontrols/nvt.cpp \
    LammpsWrappers/simulatorcontrols/simulatorcontrol.cpp \
    LammpsWrappers/simulatorcontrols/cpfix.cpp \
    LammpsWrappers/simulatorcontrols/cpcompute.cpp \
    LammpsWrappers/simulatorcontrols/cpvariable.cpp \
    LammpsWrappers/simulatorcontrols/rdf.cpp \
    LammpsWrappers/atoms.cpp \
    LammpsWrappers/system.cpp \
    LammpsWrappers/modifiers/modifier.cpp \
    LammpsWrappers/modifiers/colormodifier.cpp \
    LammpsWrappers/modifiers/slice.cpp \
    LammpsWrappers/modifiers/periodicimages.cpp \
    LammpsWrappers/atomdata.cpp \
    LammpsWrappers/regions.cpp \
    LammpsWrappers/groups.cpp \
    LammpsWrappers/neighborlist.cpp \
    LammpsWrappers/bonds.cpp \
    LammpsWrappers/lammpserror.cpp \
    LammpsWrappers/computes.cpp \
    LammpsWrappers/variables.cpp \
    LammpsWrappers/modifiers/groupmodifier.cpp \
    LammpsWrappers/modifiers/regionmodifier.cpp \
    LammpsWrappers/units.cpp \
    LammpsWrappers/fixes.cpp \
    LammpsWrappers/modifiers/propertymodifier.cpp \
    dataproviders/dataprovider.cpp \
    codeeditorbackend.cpp \
    linenumbers.cpp \
    dataproviders/data2d.cpp \
    windowgl2.cpp \
    mousemover.cpp \
    states.cpp \
    dataproviders/data1d.cpp \
    commandparser.cpp \
    performance.cpp \
    parsefileuploader.cpp \
    standardpaths.cpp \
    keysequence.cpp

HEADERS += \
    mysimulator.h \
    lammpscontroller.h \
    highlighter.h \
    datasource.h \
    LammpsWrappers/simulatorcontrols/nvt.h \
    LammpsWrappers/simulatorcontrols/simulatorcontrol.h \
    LammpsWrappers/simulatorcontrols/cpfix.h \
    LammpsWrappers/simulatorcontrols/cpcompute.h \
    LammpsWrappers/simulatorcontrols/cpvariable.h \
    LammpsWrappers/simulatorcontrols/rdf.h \
    LammpsWrappers/atoms.h \
    LammpsWrappers/system.h \
    LammpsWrappers/modifiers/modifier.h \
    LammpsWrappers/modifiers/colormodifier.h \
    LammpsWrappers/modifiers/modifiers.h \
    LammpsWrappers/modifiers/slice.h \
    LammpsWrappers/modifiers/periodicimages.h \
    LammpsWrappers/atomdata.h \
    LammpsWrappers/regions.h \
    LammpsWrappers/groups.h \
    LammpsWrappers/neighborlist.h \
    LammpsWrappers/bonds.h \
    LammpsWrappers/lammpserror.h \
    LammpsWrappers/computes.h \
    LammpsWrappers/variables.h \
    LammpsWrappers/modifiers/groupmodifier.h \
    LammpsWrappers/modifiers/regionmodifier.h \
    LammpsWrappers/units.h \
    LammpsWrappers/fixes.h \
    LammpsWrappers/modifiers/propertymodifier.h \
    dataproviders/dataprovider.h \
    codeeditorbackend.h \
    linenumbers.h \
    dataproviders/data2d.h \
    windowgl2.h \
    mousemover.h \
    states.h \
    dataproviders/data1d.h \
    commandparser.h \
    performance.h \
    parsefileuploader.h \
    standardpaths.h \
    keysequence.h

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
