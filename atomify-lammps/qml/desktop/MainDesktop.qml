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

    property alias simulator: visualizer.simulator
    property alias visualizer: visualizer

    Component.onCompleted: {
        simulator.errorInLammpsScript.connect(editorTab.reportError)
    }

    SplitView {
        anchors.fill: parent
        Layout.alignment: Qt.AlignTop
        Layout.fillHeight: true
        Layout.fillWidth: true

        TabView {
            id: tabview
            // width: desktopRoot.width*0.4
            Layout.fillHeight: true
            Layout.fillWidth: true
            width: 1
            // height: parent.height

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
                    visualizer: desktopRoot.visualizer
                    onVisualizerChanged: {
                        console.log("Visualizer: ", visualizer)
                    }
                }
            }

            Tab {
                id: renderingTab
                anchors.fill: parent
                title: "Rendering"
                RenderingTab {
                    anchors.fill: parent
                    atomifyVisualizer: visualizer
                    system: simulator.system
                    onItemsChanged: {
                        for(var i in items) {
                            var controller = items[i]
                            for(var j in controller.simulatorControls) {
                                var control = controller.simulatorControls[j]
                                control.parent = simulator
                            }
                        }
                    }
                }
            }
        }

        AtomifyVisualizer {
            id: visualizer
            focus: true
            Layout.alignment: Qt.AlignLeft
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.minimumWidth: desktopRoot.width*0.5
            //width: desktopRoot.width*0.6
        }

        SimulationSummary {
            system: simulator.system ? simulator.system : null
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.maximumWidth: 300
            Layout.minimumWidth: 300
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
}
