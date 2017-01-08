TEMPLATE = app
CONFIG += c++11
DEFINES += LAMMPS_GZIP

QT += qml quick widgets opengl openglextensions svg charts datavisualization

unix:!macx {
    QMAKE_CXXFLAGS += -fopenmp
    LIBS += -fopenmp
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
    simulations/silica/silica.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)
#include(../SimVis/examples/examples_vendor.pri) # replace with qpm installation
include(../libs/SimVis/package_vendor.pri)
include(../libs/QmlPreviewer/qmlpreviewer.pri)

!exists(examples/examples.qrc) {
    system(cd examples; python generateExamples.py)
}
RESOURCES += examples/examples.qrc

ios {
    QMAKE_INFO_PLIST = iOS.plist
}

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

DISTFILES += \
    iOS.plist \
    qml/qtquickcontrols2.conf \
    ../.travis.yml \
    ../snappy-qt5.conf \
    ../atomify-lammps.wrapper \
    ../snapcraft.yaml \
    ../configure.py \
    qml/qmldir

STATECHARTS +=
