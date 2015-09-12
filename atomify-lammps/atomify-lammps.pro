TEMPLATE = app
CONFIG += c++11
DEFINES += LAMMPS_GZIP

QT += qml quick widgets opengl openglextensions
QMAKE_CXXFLAGS += -Wno-unused-parameter -Wno-unused-variable
QMAKE_CXXFLAGS += -g
CONFIG += warn_off

# MPI STUBS
INCLUDEPATH += /projects/lammps-10Aug15/src/STUBS
DEPENDPATH += /projects/lammps-10Aug15/src/STUBS
LIBS += -L/projects/lammps-10Aug15/src/STUBS -lmpi_stubs
PRE_TARGETDEPS += /projects/lammps-10Aug15/src/STUBS/libmpi_stubs.a

CONFIG += openmp

openmp {
    # LAMMPS Library
    DEFINES += _OPENMP
    INCLUDEPATH += /projects/lammps-10Aug15/src
    DEPENDPATH += /projects/lammps-10Aug15/src
    unix: LIBS += -L/projects/lammps-10Aug15/src -llammps_omp
    unix: PRE_TARGETDEPS += /projects/lammps-10Aug15/src/liblammps_omp.a
    QMAKE_CXXFLAGS += -fopenmp
    QMAKE_LFLAGS += -fopenmp
}

#!openmp {
#    # LAMMPS Library
#    INCLUDEPATH += /projects/lammps-10Aug15/src/STUBS
#    DEPENDPATH += /projects/lammps-10Aug15/src/STUBS
#    INCLUDEPATH += /projects/lammps-10Aug15/src
#    DEPENDPATH += /projects/lammps-10Aug15/src
#    unix: LIBS += -L/projects/lammps-10Aug15/src -llammps_omp4
#    unix: PRE_TARGETDEPS += /projects/lammps-10Aug15/src/liblammps_omp.a
#}



android {
    DEFINES += LAMMPS_XDR
}

RESOURCES += qml.qrc \
    images.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)
include(../library_deployment.pri)


ios {
    QMAKE_INFO_PLIST = iOS.plist
}

SOURCES += \
    main.cpp \
    mysimulator.cpp \
    lammpscontroller.cpp \
    highlighter.cpp \
    CPcompute.cpp \
    lammpsoutput.cpp \
    lammpsfilehandler.cpp \
    atomstyle.cpp

HEADERS += \
    mysimulator.h \
    lammpscontroller.h \
    highlighter.h \
    CPcompute.h \
    lammpsoutput.h \
    lammpsfilehandler.h \
    atomstyle.h

DISTFILES += \
    iOS.plist
