import QtQuick 2.9
import QtQuick.Controls 2.2
import Atomify 1.0
Row {
    id: root
    property SimulatorControl simulatorControl
    property int numPerAtomValues: simulatorControl ? simulatorControl.numPerAtomValues : 0
    property bool hasScalarData: simulatorControl ? simulatorControl.hasScalarData : false

    Label {
        id: titleLabel
        font.underline: simulatorControl.interactive
        color: simulatorControl.interactive ? "steelblue" : "white"
        text: simulatorControl.identifier
        MouseArea {
            anchors.fill: parent
            cursorShape: simulatorControl.interactive ? Qt.PointingHandCursor : Qt.ArrowCursor
            hoverEnabled: true
            onClicked: {
                if(simulatorControl.interactive) {
                    var point = Qt.point(mouseX, mouseY)
                    point = getGlobalPosition(point, titleLabel)
                    createPlotWindow(simulatorControl, point)
                }
            }
            onEntered: simulatorControl.hovered = true
            onExited: simulatorControl.hovered = false
        }
    }
    Label {
        visible: hasScalarData || numPerAtomValues > 1
        color: (numPerAtomValues > 1) ? "steelblue" : "white"
        text: {
            if(hasScalarData) {
                if (simulatorControl.scalarValue < 0.0005) {
                    ": "+simulatorControl.scalarValue.toLocaleString(Qt.locale("en_US"),'e',3);
                } else {
                    ": "+simulatorControl.scalarValue.toPrecision(4)
                }
            } else if(numPerAtomValues > 1) {
                "   ["+(simulatorControl.perAtomIndex+1)+"]"
            } else ""
        }

        MouseArea {
            anchors.fill: parent
            enabled: numPerAtomValues > 1
            onClicked: menu.open()
        }

        Menu {
            id: menu

            Column {
                Repeater {
                    model: numPerAtomValues
                    MenuItem {
                        text: model.index+1
                        onClicked: {
                            simulatorControl.perAtomIndex = (parseInt(text)-1)
                            menu.close()
                        }
                    }
                }
            }
        }
    }
}
