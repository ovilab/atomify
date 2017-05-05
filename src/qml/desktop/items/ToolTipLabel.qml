import QtQuick 2.7
import QtQuick.Controls 2.1

Label {
    property string toolTipText: ""
    MouseArea {
        id: mouseArea
        anchors.fill: parent
        propagateComposedEvents: true
        hoverEnabled: true
        onClicked: {
            mouse.accepted = false
        }
    }
    ToolTip.text: toolTipText
    ToolTip.visible: mouseArea.containsMouse && toolTipText.length>0
}
