import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Window 2.0
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0
import MySimulator 1.0
import SimVis 1.0
import Compute 1.0
import LammpsOutput 1.0
import AtomStyle 1.0
import Qt.labs.settings 1.0

ApplicationWindow {
    id: applicationRoot

    property string mode: "desktop"

    title: qsTr("Atomify")
    width: 1650
    height: 900
    visible: true

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

    MySimulator {
        id: mySimulator
        simulationSpeed: 1
        atomStyle: AtomStyle {
            id: myAtomStyle
        }
        onErrorInLammpsScript: {
            editorTab.consoleOutput.append(" Simulation crashed. Error in parsing LAMMPS command: '"+mySimulator.scriptHandler.currentCommand+"'")
            editorTab.consoleOutput.append(" LAMMPS error message: '"+mySimulator.lammpsErrorMessage+"'")
        }
    }

    Button {
        text: "Switch mode"
        onClicked: (mode === "desktop") ? mode = "mobile" : mode = "desktop"
        anchors {
            right: parent.right
            top: parent.top
        }
    }
}
