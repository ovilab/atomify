import QtQuick 2.5
import "qrc:/mobile/style"

Item {
    id: buttonRoot
    signal clicked
    property alias text: buttonText.text
    property real maximumRotation: Math.random() * 5 + 5

    smooth: true

    transform: Rotation { id: rotationTransform
        origin.x: 30
        origin.y: 30
        axis {
            x: 0
            y: 1
            z: 0
        }
        angle: 0
    }

    Rectangle {
        anchors.fill: parent
        color: Qt.rgba(0.15, 0.15, 0.15, 0.9)
        antialiasing: true
    }

    Text {
        id: buttonText
        anchors {
            left: parent.left
            leftMargin: Style.touchableSize
            verticalCenter: parent.verticalCenter
        }

        text: "Begin"
        font.pixelSize: Style.font.button.size
        font.weight: Font.Light
        font.family: "Roboto"
        renderType: Text.QtRendering
        color: Style.font.color
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            buttonRoot.clicked()
        }
    }
}
