import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Window 2.0
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0
import AtomifySimulator 1.0
import SimVis 1.0
import Compute 1.0
import LammpsOutput 1.0
import AtomStyle 1.0
import NVT 1.0
import Qt.labs.settings 1.0

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
        onErrorInLammpsScript: {
            console.log(" Simulation crashed. Error in parsing LAMMPS command: '"+mySimulator.scriptHandler.currentCommand+"'")
            console.log(" LAMMPS error message: '"+mySimulator.lammpsErrorMessage+"'")
        }

        NVT {
            id: nvt
            temperatureDampening: 0.1
            onEnabledChanged: {
                nvtCheck.checked = enabled
            }
        }
    }

    CheckBox {
        id: nvtCheck
        anchors.top: parent.top
        anchors.right: nvtSlider.left
        checked: nvt.enabled
        onCheckedChanged: {
            nvt.enabled = checked
        }
    }

    Slider {
        id: nvtSlider
        minimumValue: 0.1
        maximumValue: 7
        anchors.right: switchButton.left
        value: nvt.targetTemperature
        onValueChanged: {
            nvt.targetTemperature = value
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
