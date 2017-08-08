TEMPLATE = app

CONFIG += c++11

QT += qml quick widgets 3drender_private

SOURCES += \
    cylindersimulator.cpp \
    main_cylinders.cpp

RESOURCES += \
    cylinders_qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(../../package_vendor.pri)

HEADERS += \
    cylindersimulator.h

DISTFILES += \
    SimpleMorphology.nml
