import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Window 2.0
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0
import Atomify 1.0
import SimVis 1.0
import Qt.labs.settings 1.0
import QMLPlot 1.0

import "mobile"
import "mobile/style"
import "desktop"

ApplicationWindow {
    id: applicationRoot

    property string mode: "mobile"

    title: qsTr("Atomify")
    width: 1650
    height: 900
    visible: true

    function resetStyle() {
        Style.reset(width, height, Screen.pixelDensity)
    }

    onWidthChanged: {
        resetStyle()
    }

    onHeightChanged: {
        resetStyle()
    }

    Component.onCompleted: {
        resetStyle()
    }

    MainDesktop {
        visible: mode === "desktop"
        anchors.fill: parent
        simulator: mySimulator
    }

    MainMobile {
        visible: mode === "mobile"
        anchors.fill: parent
        simulator: mySimulator
    }

    AtomifySimulator {
        id: mySimulator
        simulationSpeed: 1
        atomStyle: AtomStyle {
            id: myAtomStyle
        }

        NVT {
            id: nvt
            temperatureDampening: 0.01
            onEnabledChanged: {
                nvtCheck.checked = enabled
            }
        }
        scriptHandler: ScriptHandler {
            atomStyle: myAtomStyle
        }
    }

    Compute {
        property real maxValue: 0
        id: computeTemp
        identifier: "temp"
        command: "compute temp all temp"
        simulator: mySimulator
        onValueChanged: {
            // tempGraph.addPoint(mySimulator.simulationTime, 0.2+0.8*Math.sin(mySimulator.simulationTime))
            tempGraph.addPoint(mySimulator.simulationTime, value)
            tempPlot.xMax = mySimulator.simulationTime
            tempPlot.xMin = mySimulator.simulationTime-1
            maxValue = Math.max(maxValue, value)
            tempPlot.yMax = maxValue
        }
    }

    Rectangle {
        width: 400
        height: 400
        anchors.right: parent.right
        anchors.top: stuff.bottom
        Figure {
            id: tempPlot
            anchors.fill: parent
            yMin: -1
            yMax: 1
            xLabel: "t [ps] "
            yLabel: "T"
            title: "Temperature"
            LineGraph {
                id: tempGraph
            }
        }
    }

    Rectangle {
        id: stuff
        width: 300
        height: 50
        color: "white"
        anchors.right: switchButton.left
        anchors.top: parent.top
        Row {
            CheckBox {
                id: nvtCheck
                checked: nvt.enabled
                onCheckedChanged: {
                    nvt.enabled = checked
                }
            }

            Slider {
                id: nvtSlider
                minimumValue: 0.1
                maximumValue: 6
                value: nvt.targetTemperature
                onValueChanged: {
                    nvt.targetTemperature = value
                }
            }
        }
    }

    Button {
        id: switchButton
        text: "Switch mode"
        onClicked: (mode === "desktop") ? mode = "mobile" : mode = "desktop"
        anchors {
            right: parent.right
            top: parent.top
        }
    }

    Shortcut {
        sequence: StandardKey.FullScreen
        context: Qt.ApplicationShortcut
        onActivated: {
            if(visibility === Window.FullScreen) {
                visibility = Window.AutomaticVisibility
            } else {
                visibility = Window.FullScreen
            }
        }
    }
}
