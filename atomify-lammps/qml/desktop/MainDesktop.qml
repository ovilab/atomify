import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Window 2.0
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0
import Atomify 1.0
import SimVis 1.0
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
                property TextArea consoleOutput: item ? item.consoleOutput : null
                property LammpsEditor lammpsEditor: item ? item.lammpsEditor : null
                function runScript() {
                    if(lammpsEditor) {
                        lammpsEditor.runScript()
                    }
                }

                function reportError() {
                    if(item) {
                        item.reportError()
                    }
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
                    atomifyVisualizer: visualizer
                }
            }
        }

        AtomifyVisualizer {
            id: visualizer
            simulator: desktopRoot.simulator
            Layout.alignment: Qt.AlignLeft
            Layout.fillHeight: true
            Layout.fillWidth: true

            FlyModeNavigator {
                id: navigator
                anchors.fill: parent
                camera: visualizer.camera
            }

            SimulationSummary {
                simulator: desktopRoot.simulator
//                pressure: pressure
//                temperature: temperature
            }

            Rectangle {
                id: scaleRectangle
                width: parent.width
                anchors.bottom: visualizer.bottom

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
                        maximumValue: 1000
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
                        maximumValue: 10.0
                        value: visualizer.scale
                        onValueChanged: {
                            visualizer.scale = value
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
            onActivated: editorTab.runScript()
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
        Shortcut {
            sequence: "1"
            onActivated: {
                var isVisible = simulator.atomStyle.isVisible(0)
                simulator.atomStyle.setModelData(0, "visible", !isVisible)
            }
        }
        Shortcut {
            sequence: "2"
            onActivated: {
                var isVisible = simulator.atomStyle.isVisible(1)
                simulator.atomStyle.setModelData(1, "visible", !isVisible)
            }
        }
        Shortcut {
            sequence: "3"
            onActivated: {
                var isVisible = simulator.atomStyle.isVisible(2)
                simulator.atomStyle.setModelData(2, "visible", !isVisible)
            }
        }
    }

    Button {
        text: "Screenshot"
        onClicked: {
            visualizer.visualizer.grabScreenShot()
            console.log("test")
        }
    }

//    Temperature {
//        id: temperature
//        lammps: lammpsObject
//        targetValue: slider.value
//    }

//    Compute {
//        id: pressure
//        simulator: mySimulator
//        identifier: "pressure"
//        command: "compute pressure all pressure temperature"
//        dependencies: ["temperature"]
//    }
}
