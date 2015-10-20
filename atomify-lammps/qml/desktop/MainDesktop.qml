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
import Qt.labs.settings 1.0

import "../visualization"

Item {
    id: desktopRoot

    property AtomifySimulator simulator: null

    Component.onCompleted: {
        simulator.errorInLammpsScript.connect(editorTab.reportError)
    }

    SplitView {
        anchors.fill: parent
        Layout.alignment: Qt.AlignTop
        Layout.fillHeight: true

        TabView {
            id: tabview
            width: desktopRoot.width*0.4
            height: parent.height

            Tab {
                id: editorTab
                title: "Script editor"
                anchors.fill: parent
                property TextArea consoleOutput: item.consoleOutput
                property LammpsEditor lammpsEditor: item.lammpsEditor
                function reportError() {
                    item.reportError()
                }

                EditorTab {
                    id: myEditorTab
                    anchors.fill: parent
                    simulator: desktopRoot.simulator
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
            simulator: desktopRoot.simulator
            Layout.alignment: Qt.AlignLeft
            Layout.fillHeight: true
            Layout.fillWidth: true

            SimulationSummary {
                simulator: desktopRoot.simulator
                pressure: pressure
                temperature: temperature
            }

            Rectangle {
                id: scaleRectangle
                width: parent.width
                anchors.bottom: myVisualizer.bottom

                height: 25
                radius: 4
                color: Qt.rgba(1.0, 1.0, 1.0, 0.75)

                RowLayout {
                    id: scaleRow
                    x: 10
                    y: 3
                    spacing: 10

                    Label {
                        id: speedLabel
                        text: "Simulation speed"
                    }

                    Slider {
                        id: speedSlider
                        minimumValue: 1
                        maximumValue: 100
                        stepSize: 1
                        value: simulator.simulationSpeed
                        onValueChanged: {
                            if(simulator) {
                                simulator.simulationSpeed = value
                            }
                        }
                        Settings {
                            property alias speedValue: speedSlider.value
                        }
                    }

                    Label {
                        text: "Sphere size"
                    }

                    Slider {
                        id: scaleSlider
                        minimumValue: 0.1
                        maximumValue: 1.0
                        value: myVisualizer.scale
                        onValueChanged: {
                            myVisualizer.scale = value
                        }
                        Settings {
                            property alias scaleValue: scaleSlider.value
                        }
                    }
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
                visible: simulator.paused
            }
        }
    }

    Item {
        id: shortcutes
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
                simulator.paused = !simulator.paused
            }
        }
    }

//    Temperature {
//        id: temperature
//        lammps: lammpsObject
//        targetValue: slider.value
//    }

//    Compute {
//        id: temperature
//        simulator: mySimulator
//        identifier: "temperature"
//        command: "compute temperature all temp"
//    }

//    Compute {
//        id: pressure
//        simulator: mySimulator
//        identifier: "pressure"
//        command: "compute pressure all pressure temperature"
//        dependencies: ["temperature"]
//    }
}
