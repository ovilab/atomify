import QtQuick 2.2

Item {
    id: applicationRoot
    width: 1650
    height: 900
    visible: true

    SpheresScene {
        id: scene
        anchors.fill: parent
        focus: true
    }
}

