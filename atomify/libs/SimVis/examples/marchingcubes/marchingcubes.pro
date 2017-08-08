TEMPLATE = app
CONFIG += c++11

QT += qml quick widgets opengl openglextensions

SOURCES += \
    main_marchingcubes.cpp \
    marchingcubessimulator.cpp

RESOURCES += \
    marchingcubes_qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(../examples_vendor.pri)

HEADERS += \
    marchingcubessimulator.h
