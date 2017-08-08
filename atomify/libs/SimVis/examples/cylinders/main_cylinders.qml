import QtQuick 2.4

Item {
    width: 1280
    height: 720
    visible: true

    CylinderScene {
        id: scene
        anchors.fill: parent
        focus: true
    }

    MouseArea {
        property real angle: 0
//        enabled: false
        anchors.fill: parent
        onPositionChanged: {
            angle += 0.1
            scene.rotate(angle)
        }
    }
}
