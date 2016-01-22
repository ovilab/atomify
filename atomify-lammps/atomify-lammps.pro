TEMPLATE = app
CONFIG += c++11
DEFINES += LAMMPS_GZIP

QT += qml quick widgets opengl openglextensions svg
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
    LIBS += -fopenmp
}
RESOURCES += images.qrc \
    scripts.qrc \
    qml/qml.qrc \
    simulations/diffusion/diffusion.qrc \
    simulations/other/other.qrc \
    simulations/water/water.qrc \
    simulations/potentialparameters/potentialparameters.qrc

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
    scriptcommand.cpp \
    LammpsWrappers/nvt.cpp \
    LammpsWrappers/simulatorcontrol.cpp \
    LammpsWrappers/cpfix.cpp \
    LammpsWrappers/cpcompute.cpp \
    LammpsWrappers/cpvariable.cpp \
    LammpsWrappers/fixaveragetime.cpp \
    QMLPlot/figure.cpp \
    QMLPlot/graph.cpp \
    QMLPlot/linegraph.cpp \
    LammpsWrappers/fixhistogram.cpp

HEADERS += \
    mysimulator.h \
    lammpscontroller.h \
    highlighter.h \
    atomstyle.h \
    fileio.h \
    scriptparser.h \
    scripthandler.h \
    scriptcommand.h \
    LammpsWrappers/nvt.h \
    LammpsWrappers/simulatorcontrol.h \
    LammpsWrappers/cpfix.h \
    LammpsWrappers/cpcompute.h \
    LammpsWrappers/cpvariable.h \
    LammpsWrappers/fixaveragetime.h \
    QMLPlot/figure.h \
    QMLPlot/graph.h \
    QMLPlot/linegraph.h \
    LammpsWrappers/fixhistogram.h

DISTFILES += \
    iOS.plist \
    simulations/diffusion/indent/indent.png \
    simulations/diffusion/indent/indent.in \
    simulations/diffusion/indent/indent.qml
