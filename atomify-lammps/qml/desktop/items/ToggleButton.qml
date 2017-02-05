import QtQuick 2.7
import QtQuick.Controls 1.4 as QQC1
import QtQuick.Controls 2.1
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
    property string toolTipText: ""
    property real scale: 1.0
    height: buttonColumn.height

    ToolTip.text: toolTipText
    ToolTip.visible: mouseArea.containsMouse && toolTipText.length>0

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
        Label {
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
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        onClicked: {
            root.clicked()
        }
    }
}
