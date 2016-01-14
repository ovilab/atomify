TEMPLATE = app
CONFIG += c++11
DEFINES += LAMMPS_GZIP

QT += qml quick widgets opengl openglextensions
QMAKE_CXXFLAGS += -Wno-unused-parameter -Wno-unused-variable
QMAKE_CXXFLAGS += -g -fopenmp
CONFIG += warn_off

# lammps.pri includes the path and libs to lammps
# Run configure.py to generate lammps.pri

android {
    DEFINES += LAMMPS_XDR
    SOURCES +=
    HEADERS +=
    include(../lammps-android.pri)
} else {
    include(../lammps.pri)
    LIBS += -fopenmp
}

RESOURCES += images.qrc \
    scripts.qrc \
    qml/qml.qrc \
    simulations/diffusion/diffusion.qrc \
    simulations/other/other.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)
include(../SimVis/library_deployment.pri)


ios {
    QMAKE_INFO_PLIST = iOS.plist
}

SOURCES += \
    main.cpp \
    mysimulator.cpp \
    lammpscontroller.cpp \
    highlighter.cpp \
    atomstyle.cpp \
    fileio.cpp \
    scriptparser.cpp \
    scripthandler.cpp \
    nvt.cpp \
    simulatorcontrol.cpp \
    scriptcommand.cpp \
    linegraph.cpp \
    graph.cpp \
    figure.cpp \
    cpfix.cpp \
    cpcompute.cpp \
    cpvariable.cpp

HEADERS += \
    mysimulator.h \
    lammpscontroller.h \
    highlighter.h \
    atomstyle.h \
    fileio.h \
    scriptparser.h \
    scripthandler.h \
    nvt.h \
    simulatorcontrol.h \
    scriptcommand.h \
    linegraph.h \
    graph.h \
    figure.h \
    cpfix.h \
    cpcompute.h \
    cpvariable.h

DISTFILES += \
    iOS.plist \
    simulations/diffusion/indent/indent.png \
    simulations/diffusion/indent/indent.in \
    simulations/diffusion/indent/indent.qml
