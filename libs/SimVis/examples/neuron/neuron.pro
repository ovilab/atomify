TEMPLATE = app

CONFIG += c++11

QT += qml quick widgets

SOURCES += \
    neuronreader.cpp \
    main_neuron.cpp \
    instancebuffer.cpp

RESOURCES += \
    neuron_qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(../../package_vendor.pri)

HEADERS += \
    neuronreader.h \
    instancebuffer.h

DISTFILES += \
    SimpleMorphology.nml \
    instanced.frag \
    instanced.vert
