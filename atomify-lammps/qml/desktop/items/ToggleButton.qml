import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0
import Qt.labs.settings 1.0

Item {
    id: root

    signal clicked

    property alias toggled: toggleImage.toggled
    property alias source: toggleImage.source
    property alias text: buttonText.text

    height: buttonColumn.height
    Column {
        id: buttonColumn
        anchors {
            left: parent.left
            right: parent.right
        }
        spacing: 8
        ToggleImage {
            id: toggleImage
            anchors {
                left: parent.left
                right: parent.right
                margins: 8
            }
            height: width
        }
        Text {
            id: buttonText
            anchors {
                left: parent.left
                right: parent.right
            }

            color: "#ccc"
            text: name
            horizontalAlignment: Text.AlignHCenter
        }
    }
    MouseArea {
        anchors.fill: parent
        onClicked: {
            root.clicked()
        }
    }
}
