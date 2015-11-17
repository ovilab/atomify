import QtQuick 2.5

Item {
    id: controlRoot
    signal clicked

    property list<Item> fixes
    property list<Item> computes
    property Component fullControl

    MouseArea {
        anchors.fill: parent
        onClicked: {
            controlRoot.clicked()
        }
    }
}

