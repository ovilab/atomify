TEMPLATE = app
CONFIG += c++11
DEFINES += LAMMPS_GZIP

QT += qml quick widgets opengl openglextensions svg charts
QMAKE_CXXFLAGS += -Wno-unused-parameter -Wno-unused-variable
QMAKE_CXXFLAGS += -g -fopenmp
LIBS += -fopenmp
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
#include(../SimVis/examples/examples_vendor.pri) # replace with qpm installation
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
    datasource.cpp \
    LammpsWrappers/simulatorcontrols/nvt.cpp \
    LammpsWrappers/simulatorcontrols/simulatorcontrol.cpp \
    LammpsWrappers/simulatorcontrols/cpfix.cpp \
    LammpsWrappers/simulatorcontrols/cpcompute.cpp \
    LammpsWrappers/simulatorcontrols/cpvariable.cpp \
    LammpsWrappers/simulatorcontrols/fixaveragetime.cpp \
    LammpsWrappers/simulatorcontrols/fixhistogram.cpp \
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
    dataproviders/dataprovider.cpp \
    dataproviders/cp1ddata.cpp \
    LammpsWrappers/variables.cpp \
    LammpsWrappers/modifiers/groupmodifier.cpp \
    LammpsWrappers/modifiers/regionmodifier.cpp \
    codeeditorbackend.cpp \
    linenumbers.cpp \
    LammpsWrappers/units.cpp

HEADERS += \
    mysimulator.h \
    lammpscontroller.h \
    highlighter.h \
    scriptparser.h \
    scripthandler.h \
    scriptcommand.h \
    datasource.h \
    LammpsWrappers/simulatorcontrols/nvt.h \
    LammpsWrappers/simulatorcontrols/simulatorcontrol.h \
    LammpsWrappers/simulatorcontrols/cpfix.h \
    LammpsWrappers/simulatorcontrols/cpcompute.h \
    LammpsWrappers/simulatorcontrols/cpvariable.h \
    LammpsWrappers/simulatorcontrols/fixaveragetime.h \
    LammpsWrappers/simulatorcontrols/fixhistogram.h \
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
    dataproviders/dataprovider.h \
    dataproviders/cp1ddata.h \
    LammpsWrappers/variables.h \
    LammpsWrappers/modifiers/groupmodifier.h \
    LammpsWrappers/modifiers/regionmodifier.h \
    codeeditorbackend.h \
    linenumbers.h \
    LammpsWrappers/units.h

# Temporary use of quickcontrols2 without install


DISTFILES += \
    iOS.plist \
    simulations/diffusion/indent/indent.png \
    simulations/diffusion/indent/indent.in \
    simulations/diffusion/indent/indent.qml \
    ../.travis.yml \
    qml/qtquickcontrols2.conf
