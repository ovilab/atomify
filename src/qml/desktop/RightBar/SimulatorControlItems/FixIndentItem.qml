import QtQuick 2.9
import QtQuick.Controls 2.2
import Atomify 1.0
Row {
    id: root
    property SimulatorControl simulatorControl

    Label {
        color: "steelblue"
        text: simulatorControl.identifier
        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            onEntered: simulatorControl.hovered = true
            onExited: simulatorControl.hovered = false
        }
    }
}
