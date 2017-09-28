import QtQuick 2.9
import QtQuick.Controls 2.2
import Atomify 1.0

Row {
    id: root
    property SimulatorControl simulatorControl
    property int numPerAtomValues: simulatorControl.numPerAtomValues
    property bool hasScalarData: simulatorControl.hasScalarData

    Label {
        id: titleLabel
        font.underline: true
        color: "steelblue"
        text: simulatorControl.identifier
        MouseArea {
            anchors.fill: parent
            cursorShape: Qt.PointingHandCursor
            hoverEnabled: true
            onClicked: {
                var point = Qt.point(mouseX, mouseY)
                point = getGlobalPosition(point, titleLabel)
                createPlotWindow(simulatorControl, point)
            }
            onEntered: simulatorControl.hovered = true
            onExited: simulatorControl.hovered = false
        }
    }
    Label {
        text: {
            if(!simulatorControl.hasScalarData) return ""
            var value = simulatorControl.scalarValue
            var valueHasDecimals = value !== Math.round(value, 0)
            if(valueHasDecimals) {
                if (value < 0.0005) {
                    ": "+value.toLocaleString(Qt.locale("en_US"),'e',3);
                } else {
                    ": "+value.toPrecision(4)
                }
            } else {
                ": "+value.toFixed(0)
            }
        }
    }
}
