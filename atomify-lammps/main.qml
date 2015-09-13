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
                    id: myLammpsEditor
                    anchors.fill: parent
                    simulator: mySimulator
                    Shortcut {
                        // Random placement here because it could not find the editor otherwise (Qt bug?)
                        sequence: "Ctrl+R"
                        onActivated: runScript()
                    }
                    Shortcut {
                        sequence: "Escape"
                        onActivated: {
                            if(textarea.focus) textarea.focus = false
                            else mySimulator.paused = !mySimulator.paused
                        }
                    }
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
                width: statusColumn.width+20
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

                MouseArea {
                    anchors.fill: parent
                    drag.target: parent
                    drag.axis: Drag.XAndYAxis
                }
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
