INCLUDEPATH += $$PWD
INCLUDEPATH += $$PWD/../src

HEADERS += \
    $$PWD/vendor.h

!ios {
#    OBJECTS_DIR=$$shadowed($$PWD)/.obj
#    MOC_DIR=$$shadowed($$PWD)/.moc
}

include(../package.pri)
