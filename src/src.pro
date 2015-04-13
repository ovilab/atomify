TEMPLATE = lib
TARGET = SimVis
CONFIG += qt plugin c++11

QMAKE_CXXFLAGS += -g

ios {
    CONFIG += static
    DEFINES += STATIC_BUILD
    QMAKE_MOC_OPTIONS += -Muri=SimVis
}

TARGET = $$qtLibraryTarget($$TARGET)
uri = com.mycompany.qmlcomponents

QT += qml quick widgets opengl openglextensions

SOURCES += \
    core/camera.cpp \
    core/navigator.cpp \
    core/quickworker.cpp \
    core/renderable.cpp \
    core/simulator.cpp \
    core/simvisplugin.cpp \
    navigators/trackballnavigator.cpp \
    core/visualizer.cpp \
    core/cameralens.cpp \
    core/lookattransform.cpp \
    renderables/billboards/billboards.cpp \
    renderables/points/points.cpp \
    renderables/marchingcubes/marchingcubes.cpp \
    renderables/marchingcubes/marchingcubesgenerator.cpp \
    core/shadereffect.cpp \
    shadereffects/simplexbump.cpp \
    shadereffects/simplextexture.cpp \
    renderables/spheres/spheres.cpp \
    shadereffects/light.cpp \
    renderables/skybox/skybox.cpp \
    shadereffects/skyboxreflection.cpp

HEADERS += \
    core/navigator.h \
    core/quickworker.h \
    core/renderable.h \
    core/simulator.h \
    core/camera.h \
    core/simvisplugin.h \
    navigators/trackballnavigator.h \
    core/visualizer.h \
    core/camera_p.h \
    core/cameralens.h \
    core/cameralens_p.h \
    core/lookattransform.h \
    core/lookattransform_p.h \
    renderables/billboards/billboards.h \
    renderables/points/points.h \
    renderables/marchingcubes/marchingcubes.h \
    renderables/marchingcubes/marchingcubesgenerator.h \
    SimVis/Billboards \
    SimVis/Points \
    SimVis/MarchingCubes \
    core/shadereffect.h \
    shadereffects/simplexbump.h \
    shadereffects/simplextexture.h \
    renderables/spheres/spheres.h \
    SimVis/Spheres \
    shadereffects/light.h \
    renderables/skybox/skybox.h \
    shadereffects/skyboxreflection.h

DISTFILES = qmldir \
    core/simvisplugin.json

DESTDIR = $$OUT_PWD/../dist/SimVis
HEADERS_DESTDIR = $$OUT_PWD/../dist/include

RESOURCES += \
    shaders.qrc \
    qmldir.qrc

# Used for dynamic builds

!equals(_PRO_FILE_PWD_, $$OUT_PWD) {
    copy_qmldir.target = $$DESTDIR/qmldir
    copy_qmldir.depends = $$_PRO_FILE_PWD_/imports/SimVis/qmldir
    copy_qmldir.commands = $(COPY_FILE) \"$$replace(copy_qmldir.depends, /, $$QMAKE_DIR_SEP)\" \"$$replace(copy_qmldir.target, /, $$QMAKE_DIR_SEP)\"
    QMAKE_EXTRA_TARGETS += copy_qmldir
    PRE_TARGETDEPS += $$copy_qmldir.target
}

qmldir.files = qmldir
unix {
    installPath = $$[QT_INSTALL_QML]/$$replace(uri, \\., /)
    qmldir.path = $$installPath
    target.path = $$installPath
    INSTALLS += target qmldir
}
