import QtQuick 2.5
import QtQuick.Layouts 1.1

import "qrc:/visualization"

Item {
    id: controlRoot

    signal clicked

    property real itemSize

    Layout.fillWidth: true
    Layout.preferredHeight: itemSize * Layout.rowSpan

    width: 10
    height: 10

    MouseArea {
        anchors.fill: parent
        onClicked: {
            controlRoot.clicked()
        }
    }
}

