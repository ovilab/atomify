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
        scriptHandler: ScriptHandler {
            atomStyle: myAtomStyle
        }

//        Variable {
//            id: myVariable
//            identifier: "twoTemp"
//            command: "equal 2.0*c_"+computeTemp.identifier
//            dependencies: [computeTemp]
//        }

//        Fix {
//            id: fixAveTime
//            identifier: "tempout"
//            command: "all ave/time 10 1 10 v_"+myVariable.identifier+" file /projects/test.txt"
//            dependencies: [myVariable]
//        }

//        Fix {
//            id: gcmcFix
//            identifier: gcmc
//            command: "all gcmc 1 10 10 1 352534 1.0 -5.0 1.0"
//        }

//        Compute {
//            property real maxValue: 0
//            id: computeTemp
//            identifier: "temperature"
//            command: "all temp"
//            onValueChanged: {
//                 console.log("test" + value)

//                //tempGraph.addPoint(mySimulator.simulationTime, value)
//                //tempPlot.xMax = mySimulator.simulationTime
//                //tempPlot.xMin = mySimulator.simulationTime-1
//                //maxValue = Math.max(maxValue, value)
//                //tempPlot.yMax = maxValue
//            }
//        }
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
//    Button {
//        id: switchButton
//        text: "Switch mode"
//        onClicked: (mode === "desktop") ? mode = "mobile" : mode = "desktop"
//        anchors {
//            right: parent.right
//            top: parent.top
//        }
//    }

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
