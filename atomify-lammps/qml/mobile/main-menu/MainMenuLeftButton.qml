import QtQuick 2.0
import "qrc:/mobile/style"

Item {
    id: buttonRoot
    signal clicked
    property alias text: buttonText.text
    height: Style.font.button.size * 3
    width: parent.width
    Text {
        id: buttonText
        anchors.verticalCenter: parent.verticalCenter
        color: Style.font.color
        font.pixelSize: Style.font.button.size
    }
    MouseArea {
        anchors.fill: parent
        onClicked: buttonRoot.clicked()
    }
}
