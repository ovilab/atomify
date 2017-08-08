TEMPLATE = app
CONFIG += c++11

QT += qml quick widgets opengl openglextensions

SOURCES += \
    main_functionsurface.cpp \
    functionsurfacesimulator.cpp

RESOURCES += \
    functionsurface_qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(../examples_vendor.pri)

HEADERS += \
    functionsurfacesimulator.h

DISTFILES += \
    fs.frag \
    fs.vert \
    fs.geom
