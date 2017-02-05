import QtQuick 2.7
import QtQuick.Controls 2.1

Label {
    property string toolTipText: ""
    MouseArea { id: mouseArea; hoverEnabled: true; anchors.fill: parent }
    ToolTip.text: toolTipText
    ToolTip.visible: mouseArea.containsMouse && toolTipText.length>0
}
