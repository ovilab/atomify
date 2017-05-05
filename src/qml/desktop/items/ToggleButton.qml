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

    ToolTip.text: toolTipText
    ToolTip.visible: mouseArea.containsMouse && toolTipText.length>0
    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        onClicked: {
            root.clicked()
        }
    }

    ToggleImage {
        id: toggleImage
        anchors {
            top: parent.top
            bottom: buttonText.top
            left: parent.left
            right: parent.right
            leftMargin: 16
            rightMargin: 16
        }
    }

    Label {
        id: buttonText
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }

        color: "#ccc"
        text: name
        horizontalAlignment: Text.AlignHCenter
    }
}
