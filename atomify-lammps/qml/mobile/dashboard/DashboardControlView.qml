import QtQuick 2.5

import "qrc:/mobile/style"

Rectangle {
    signal backClicked

    property Component component

    width: parent.width
    height: parent.height
    color: "blue"
    Loader {
        anchors.fill: parent
        sourceComponent: component
    }

    Image {
        anchors {
            left: parent.left
            top: parent.top
            margins: Style.baseMargin
        }

        width: Style.touchableSize
        height: width

        source: "qrc:/images/back.png"
        smooth: true
        antialiasing: true

        MouseArea {
            anchors.fill: parent
            onClicked: backClicked()
        }
    }
}

