# Find git install
win32 {
    CUTEVERSIONING_GIT_BIN = $$system(where git)
}
unix|max {
    CUTEVERSIONING_GIT_BIN = $$system(which git)
}

CUTEVERSIONING_GIT_LATEST_TAG = "git-not-found"
CUTEVERSIONING_GIT_DESCRIPTION = "git-not-found"
CUTEVERSIONING_GIT_DIRTY = true

# If git is found, run git describe
!isEmpty(CUTEVERSIONING_GIT_BIN) {
    CUTEVERSIONING_GIT_LATEST_TAG = $$system(git --git-dir $$CUTEVERSIONING_GIT_DIR --work-tree $$CUTEVERSIONING_GIT_WORK_TREE describe --always --tags --abbrev=0)
    CUTEVERSIONING_GIT_DESCRIPTION = $$system(git --git-dir $$CUTEVERSIONING_GIT_DIR --work-tree $$CUTEVERSIONING_GIT_WORK_TREE describe --always --tags --long)
    CUTEVERSIONING_GIT_STATUS = $$system(git --git-dir $$CUTEVERSIONING_GIT_DIR --work-tree $$CUTEVERSIONING_GIT_WORK_TREE status --porcelain --untracked-files=no)
    isEmpty(CUTEVERSIONING_GIT_STATUS) {
        CUTEVERSIONING_GIT_DIRTY=false
    }
}

# Write header file
HEADER_OUTPUT = \
"$${LITERAL_HASH}include <QString>"\
"$${LITERAL_HASH}include <QVersionNumber>"\
""\
"namespace CuteVersioning {"\
"const QString latestTag = \"$${CUTEVERSIONING_GIT_LATEST_TAG}\";"\
"const QString identifier = \"$${CUTEVERSIONING_GIT_DESCRIPTION}\";"\
"const bool dirty = $${CUTEVERSIONING_GIT_DIRTY};"\
"const QVersionNumber versionNumber = QVersionNumber::fromString(latestTag);"\
"}"\

write_file($$PWD/CuteVersioning/version.h, HEADER_OUTPUT)

# Write AndroidManifest.xml
android {
    !isEmpty(CUTEVERSIONING_REPLACE_ANDROID_VERSION_NAME) {
        !isEmpty(ANDROID_PACKAGE_SOURCE_DIR) {
            ANDROID_RESULT = $$system(cat $${ANDROID_PACKAGE_SOURCE_DIR}/AndroidManifest.xml | \
                        sed -e 's/android:versionName=\\"[^\\"]*/android:versionName=\\"$${CUTEVERSIONING_GIT_LATEST_TAG}/' \
                        > $${ANDROID_PACKAGE_SOURCE_DIR}/AndroidManifest-tmp.xml && \
                        mv $${ANDROID_PACKAGE_SOURCE_DIR}/AndroidManifest-tmp.xml $${ANDROID_PACKAGE_SOURCE_DIR}/AndroidManifest.xml)
        }
    }
}

# Triggers to enable this every build
qmakeforce.target = dummy
unix {
    qmakeforce.commands = touch $$PWD/dummy.pri # hack to force qmake to run every time
} win32 {
    # TODO support for forced qmake run on Windows
}
qmakeforce.depends = FORCE
POST_TARGETDEPS += $$qmakeforce.target
QMAKE_EXTRA_TARGETS += qmakeforce

exists($$PWD/CuteVersioning/version.h) {
    HEADERS += $$PWD/CuteVersioning/version.h
}

HEADERS += \
    $$PWD/CuteVersioning/CuteVersioning \
    $$PWD/src/init.h \
    $$PWD/src/quickversion.h \
    $$PWD/CuteVersioning/cuteversioning.h

SOURCES += \
    $$PWD/src/quickversion.cpp

INCLUDEPATH += $$PWD

include(dummy.pri)
