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
import "editor"
import "../plotting"
Item {
    id: root

    property AtomifySimulator simulator: visualizer.simulator
    property alias visualizer: visualizer
    property bool focusMode: false

    Component.onCompleted: {
        // editorTab.lammpsEditor.runScript()
    }

    DropArea {
        anchors.fill: parent
        onDropped: {
            if(drop.hasUrls) {
                var numUrls = drop.urls.length
                for(var i=0; i<drop.urls.length; i++) {
                    var url = drop.urls[i]
                    editorTab.codeEditorWindow.openTab(url)

                }
            }
        }
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
                simulator: root.simulator
                visualizer: root.visualizer
                Component.onCompleted: {
                    simulator.errorInLammpsScript.connect(editorTab.reportError)
                }
                onAutomaticallyRunChanged: simulator.automaticallyRun = automaticallyRun
            }

            AtomifyVisualizer {
                id: visualizer
                Layout.alignment: Qt.AlignLeft
                Layout.fillHeight: true
                Layout.minimumWidth: 1
                focus: true
                ambientOcclusion.radius: radiusSlider.value
                ambientOcclusion.samples: samplesSlider.value
                ambientOcclusion.noiseScale: noiseScaleSlider.value
                ambientOcclusion.mode: ssaoMode.currentText
                sphereScale: sphereScalingSlider.value
                bondRadius: bondRadiusSlider.value
                periodicImages.numberOfCopiesX: periodicXSlider.value
                periodicImages.numberOfCopiesY: periodicYSlider.value
                periodicImages.numberOfCopiesZ: periodicZSlider.value
                // renderMode: deferredModeSwitch.checked ? "deferred" : "forward"
            }
        }

        RightBar {
            id: simulationSummary

            width: 300
            height: parent.height
            system: simulator.system ? simulator.system : null
            visualizer: root.visualizer

            Column {

                visible: false
                y: parent.height - 300
                width: parent.width
                Row {
                    Label {
                        width: 150
                        text: "SSAO Radius ("+radiusSlider.value.toFixed(1)+"): "
                    }

                    Slider {
                        id: radiusSlider
                        width: 150
                        minimumValue: 0.0
                        value: 8.0
                        maximumValue: 50.0
                    }
                }

                Row {
                    Label {
                        width: 150
                        text: "SSAO Noise ("+noiseScaleSlider.value.toFixed(1)+"): "
                    }

                    Slider {
                        id: noiseScaleSlider
                        width: 150
                        minimumValue: 0.0
                        value: 5.0
                        maximumValue: 10.0
                    }
                }

                Row {
                    Label {
                        width: 150
                        text: "SSAO samples ("+samplesSlider.value+"): "
                    }
                    Slider {
                        id: samplesSlider
                        width: 150
                        minimumValue: 1
                        value: 32
                        stepSize: 1
                        maximumValue: 64
                    }
                }

                Row {
                    Label {
                        width: 150
                        text: "Sphere scaling ("+sphereScalingSlider.value.toFixed(2)+"): "
                    }
                    Slider {
                        id: sphereScalingSlider
                        width: 150
                        minimumValue: 0.1
                        maximumValue: 1.0
                        value: 0.23
                        stepSize: 0.01
                    }
                }

                Row {
                    Label {
                        width: 150
                        text: "Bond radius ("+bondRadiusSlider.value.toFixed(2)+"): "
                    }
                    Slider {
                        id: bondRadiusSlider
                        width: 150
                        minimumValue: 0.1
                        maximumValue: 1.0
                        value: 0.1
                        stepSize: 0.01
                    }
                }

                Row {
                    Label {
                        width: 150
                        text: "Periodic X: "
                    }
                    Slider {
                        id: periodicXSlider
                        width: 150
                        minimumValue: 1
                        maximumValue: 5.0
                        value: 1
                        stepSize: 1
                    }
                }

                Row {
                    Label {
                        width: 150
                        text: "Periodic Y: "
                    }
                    Slider {
                        id: periodicYSlider
                        width: 150
                        minimumValue: 1
                        maximumValue: 5.0
                        value: 1
                        stepSize: 1
                    }
                }

                Row {
                    Label {
                        width: 150
                        text: "Periodic Z: "
                    }
                    Slider {
                        id: periodicZSlider
                        width: 150
                        minimumValue: 1
                        maximumValue: 5.0
                        value: 1
                        stepSize: 1
                    }
                }

                ComboBox {
                    id: ssaoMode
                    model: ["hemisphere", "sphere"]
                    currentIndex: 0
                }

                ComboBox {
                    model: ["blurMultiply", "ssaoMultiply", "blur", "ssao", "position", "color", "normal"]
                    currentIndex: 0
                    onCurrentTextChanged: {
                        visualizer.finalShaderBuilder.selectOutput(currentText)
                    }
                }


                Row {
                    Label {
                        width: 150
                        text: "Deferred: "
                    }
                    Switch {
                        id: deferredModeSwitch
                        checked: false
                    }
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
            context: Qt.ApplicationShortcut
            onActivated: editorTab.lammpsEditor.runScript()
        }
        Shortcut {
            sequence: "Ctrl+P"
            context: Qt.ApplicationShortcut
            onActivated: simulator.paused = !simulator.paused
        }
        Shortcut {
            sequence: "Space"
            context: Qt.ApplicationShortcut
            onActivated: simulator.paused = !simulator.paused
        }
        Shortcut {
            sequence: "Tab"
            context: Qt.ApplicationShortcut
            onActivated: toggleFocusMode()
        }
        Shortcut {
            sequence: "+"
            context: Qt.ApplicationShortcut
            onActivated: {
                if(simulator.simulationSpeed < 100) {
                    simulator.setSimulationSpeed(simulator.simulationSpeed+1)
                }
            }
        }
        Shortcut {
            sequence: "-"
            context: Qt.ApplicationShortcut
            onActivated: {
                if(simulator.simulationSpeed > 1) {
                    simulator.setSimulationSpeed(simulator.simulationSpeed-1)
                }
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
        simulator: root.simulator
        visualizer: root.visualizer
        visible: !root.focusMode
        x: visualizer.x + visualizer.width*0.5 - 0.5*width

        y: parent.height - 100
        width: 320
        height: 64
    }
}
