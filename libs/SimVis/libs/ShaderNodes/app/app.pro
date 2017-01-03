QT += qml quick qml-private 3dcore-private 3dquick-private

CONFIG += c++11

SOURCES += main.cpp \
    nodewrapper.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)
include(../package_vendor.pri)

HEADERS += \
    nodewrapper.h
