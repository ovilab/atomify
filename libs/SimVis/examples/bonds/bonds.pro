TEMPLATE = app
CONFIG += c++11

QT += widgets opengl openglextensions qml quick 3dcore 3drender 3dinput 3dquick

SOURCES += \
    bondsimulator.cpp \
    main_bond.cpp

RESOURCES += \
    bond_qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(../../package_vendor.pri)

HEADERS += \
    bondsimulator.h
