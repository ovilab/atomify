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
    width: 320
    height: 560
    visible: true

    function resetStyle() {
        Style.reset(width, height, Screen)
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

        Compute {
            property real maxValue: 0
            id: computeTemp
            identifier: "temperature"
            command: "all temp"
            onValueChanged: {
//            tempGraph.addPoint(mySimulator.simulationTime, value)
//            tempPlot.xMax = mySimulator.simulationTime
//            tempPlot.xMin = mySimulator.simulationTime-1
//            maxValue = Math.max(maxValue, value)
//            tempPlot.yMax = maxValue
            }
        }

    Shortcut {
        sequence: StandardKey.AddTab
        context: Qt.ApplicationShortcut
        onActivated: {
            if(mode === "desktop") {
                mode = "mobile"
            } else {
                mode = "desktop"
//                tempPlot.xMax = mySimulator.simulationTime
//                tempPlot.xMin = mySimulator.simulationTime-1
//                maxValue = Math.max(maxValue, value)
//                tempPlot.yMax = maxValue
            }
        }
    }

//    Compute {
//        property real maxValue: 0
//        id: computeMsd
//        identifier: "msd"
//        command: "compute msd all msd"
//        isVector: true
//        simulator: mySimulator
//        onFourthValueChanged: {
//            // tempGraph.addPoint(mySimulator.simulationTime, 0.2+0.8*Math.sin(mySimulator.simulationTime))
//            tempGraph.addPoint(mySimulator.simulationTime, fourthValue)
//            tempPlot.xMax = mySimulator.simulationTime
//            //tempPlot.xMin = mySimulator.simulationTime-1
//            maxValue = Math.max(maxValue, fourthValue)
//            tempPlot.yMax = maxValue
//        }
//    }

                color: "red"
                width: 3
                style: Qt.DashDotDotLine
            }
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
