#!/bin/bash
export QTDIR=/opt/qt5/5.9/gcc_64
export PATH=$QTDIR/bin:$PATH
SNAP_DUMP=snapdump
python configure.py
qmake
make -j8
mkdir -p ${SNAP_DUMP}
cp -r $QTDIR/bin ${SNAP_DUMP}/
cp -r $QTDIR/lib ${SNAP_DUMP}/
cp -r $QTDIR/plugins ${SNAP_DUMP}/
cp -r $QTDIR/plugins/platforms ${SNAP_DUMP}/
cp -r $QTDIR/qml ${SNAP_DUMP}/
cp src/atomify ${SNAP_DUMP}/bin/atomify
