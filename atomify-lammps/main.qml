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
    title: qsTr("Atomify LAMMPS - live visualization")
    width: 1650
    height: 900
    visible: true

    SplitView {
        anchors.fill: parent
        Layout.alignment: Qt.AlignTop
        Layout.fillHeight: true

        TabView {
            id: tabview
            width: applicationRoot.width*0.4
            height: parent.height

            Tab {
                id: editorTab
                title: "Script editor"
                anchors.fill: parent
                property TextArea consoleOutput: item.consoleOutput
                property LammpsEditor lammpsEditor: item.lammpsEditor
                EditorTab {
                    id: myEditorTab
                    anchors.fill: parent
                    simulator: mySimulator
                }
            }

            Tab {
                id: renderingTab
                anchors.fill: parent
                title: "Rendering"
                RenderingTab {
                    anchors.fill: parent
                    atomifyVisualizer: myVisualizer
                }
            }
        }

        AtomifyVisualizer {
            id: myVisualizer
            simulator: mySimulator
            Layout.alignment: Qt.AlignLeft
            Layout.fillHeight: true
            Layout.fillWidth: true

            SimulationSummary {
                simulator: mySimulator
                pressure: pressure
                temperature: temperature
            }

            Label {
                x: 0.5*(parent.width - width)
                y: 25
                color: Qt.rgba(1,1,1,0.5)
                width: 200
                height: 50
                font.pixelSize: 50
                text: "Paused"
                visible: mySimulator.paused
            }
        }
    }

    MySimulator {
        id: mySimulator
        simulationSpeed: 1
        atomStyle: AtomStyle {
            id: myAtomStyle
        }
        onErrorInLammpsScript: {
            editorTab.consoleOutput.append(" Simulation crashed. Error in parsing LAMMPS command: '"+mySimulator.lastCommand+"'")
            editorTab.consoleOutput.append(" LAMMPS error message: '"+mySimulator.lammpsErrorMessage+"'")
        }
    }

    Shortcut {
        // Random placement here because it could not find the editor otherwise (Qt bug?)
        sequence: "Ctrl+R"
        onActivated: editorTab.lammpsEditor.runScript()
    }
    Shortcut {
        sequence: "Ctrl+1"
        onActivated: tabview.currentIndex = 0
    }
    Shortcut {
        sequence: "Ctrl+2"
        onActivated: tabview.currentIndex = 1
    }
    Shortcut {
        sequence: "Space"
        onActivated: {
            mySimulator.paused = !mySimulator.paused
        }
    }

    Compute {
        id: temperature
        simulator: mySimulator
        identifier: "temperature"
        command: "compute temperature all temp"
    }

    Compute {
        id: pressure
        simulator: mySimulator
        identifier: "pressure"
        command: "compute pressure all pressure temperature"
        dependencies: ["temperature"]
    }
}
