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
                anchors.fill: parent
                title: "Script editor"

                LammpsEditor {
                    id: lammpsEditor
                    anchors.fill: parent
                    simulator: mySimulator
                }
            }

            Tab {
                id: renderingTab
                anchors.fill: parent
                title: "Rendering"
                Rendering {
                    anchors.fill: parent
                    simulator: mySimulator
                    light: myVisualizer.light
                }
            }
        }

        AtomifyVisualizer {
            id: myVisualizer
            simulator: mySimulator
            Layout.alignment: Qt.AlignLeft
            Layout.fillHeight: true
            Layout.fillWidth: true

            Rectangle {
                x: 20
                y: 20
                width: 200
                height: statusColumn.height+20
                radius: 4
                color: Qt.rgba(1.0, 1.0, 1.0, 0.75)
                ColumnLayout {
                    y: 10
                    x: 10
                    id: statusColumn
                    Text {
                        font.bold: true
                        text: "Number of atoms: "+mySimulator.numberOfAtoms
                    }
                    Text {
                        font.bold: true
                        text: "Number of atom types: "+mySimulator.numberOfAtomTypes
                    }

                    Text {
                        font.bold: true
                        text: "Temperature: "+temperature.value
                    }

                    Text {
                        font.bold: true
                        text: "Pressure: "+pressure.value
                    }
                }
            }
        }
    }

    MySimulator {
        id: mySimulator
        simulationSpeed: 1
        atomStyle: AtomStyle {
            id: myAtomStyle
        }
    }

    Shortcut {
        sequence: "Ctrl+1"
        onActivated: tabview.currentIndex = 0
    }
    Shortcut {
        sequence: "Ctrl+2"
        onActivated: tabview.currentIndex = 1
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
