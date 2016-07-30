import QtQuick 2.7
import QtQuick.Controls 1.5
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

    property AtomifySimulator simulator: visualizer.simulator
    property alias visualizer: visualizer
    property bool focusMode: false

    Component.onCompleted: {
        editorTab.lammpsEditor.runScript()
    }

    Row {
        anchors.fill: parent

        SplitView {
            height: parent.height
            width: parent.width-simulationSummary.width
            Layout.alignment: Qt.AlignTop
            orientation: Qt.Horizontal

            EditorTab {
                id: editorTab
                Layout.fillHeight: true
                width: 500

                simulator: desktopRoot.simulator
                visualizer: desktopRoot.visualizer
                Component.onCompleted: {
                    simulator.errorInLammpsScript.connect(editorTab.reportError)
                }
            }

            AtomifyVisualizer {
                id: visualizer
                Layout.alignment: Qt.AlignLeft
                Layout.fillHeight: true
                Layout.minimumWidth: 1
                focus: true
                ambientOcclusion.radius: radiusSlider.value
                ambientOcclusion.samples: samplesSlider.value
            }

        }

        SimulationSummary {
            id: simulationSummary
            width: 300
            height: parent.height
            system: simulator.system ? simulator.system : null

            Column {
                Slider {
                    id: radiusSlider
                    minimumValue: 0.0
                    value: 0.5
                    maximumValue: 50.0
                }
                Slider {
                    id: samplesSlider
                    minimumValue: 1
                    value: 10
                    stepSize: 1
                    maximumValue: 64
                }
            }
        }
    }

    function toggleFocusMode() {
        if(focusMode) {
            simulationSummary.width = 300
            simulationSummary.visible = true
            editorTab.visible = true
            focusMode = false
            tabDisable.hideTabDisable.start()
        } else {
            simulationSummary.width = 0
            editorTab.visible = false
            simulationSummary.visible = false
            focusMode = true
            tabDisable.showTabDisable.start()
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
            onActivated: tabView.currentIndex = 0
        }
        Shortcut {
            sequence: "Ctrl+2"
            onActivated: tabView.currentIndex = 1
        }
        Shortcut {
            sequence: "Tab"
            onActivated: toggleFocusMode()
        }
        Shortcut {
            sequence: "+"
            onActivated: {
                if(simulator.simulationSpeed < 100) {
                    simulator.setSimulationSpeed(simulator.simulationSpeed+1)
                }
            }
        }
        Shortcut {
            sequence: "-"
            onActivated: {
                if(simulator.simulationSpeed > 1) {
                    simulator.setSimulationSpeed(simulator.simulationSpeed-1)
                }
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

    DisableMessage {
        id: tabDisable
        x: parent.width*0.5 - 0.5*width
        y: 10
    }

    ControlBar {
        id: controlBar1
        simulator: desktopRoot.simulator
        visualizer: desktopRoot.visualizer
        visible: !desktopRoot.focusMode
        x: visualizer.x + visualizer.width*0.5 - 0.5*width

        y: parent.height - 100
        width: 300
        height: 50
    }
}
