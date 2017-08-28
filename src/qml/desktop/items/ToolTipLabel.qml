import QtQuick 2.7
import QtQuick.Controls 2.2

Label {
    property string toolTipText: ""
    property string url: ""
    MouseArea {
        id: mouseArea
        anchors.fill: parent
        propagateComposedEvents: true
        hoverEnabled: true
        onPressed:  {
            if(url!=="") {
                Qt.openUrlExternally(url)
            }
            mouse.accepted = false
        }
    }
    ToolTip.text: toolTipText
    ToolTip.visible: mouseArea.containsMouse && toolTipText.length>0
}
