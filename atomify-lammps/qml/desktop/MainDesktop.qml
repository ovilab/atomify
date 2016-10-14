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
    property alias renderQuality: visualizer.renderQuality
    property alias mouseMover: visualizer.mouseMover
    property bool focusMode: false


    function toggleFocusMode() {
        if(focusMode) {
            rightbar.width = 300
            rightbar.visible = true
            editor.visible = true
            focusMode = false
            tabDisable.hideTabDisable.start()
        } else {
            rightbar.width = 0
            editor.visible = false
            rightbar.visible = false
            focusMode = true
            tabDisable.showTabDisable.start()
        }
    }


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
                    editor.editorWindow.openTab(url)
                }
            }
        }
    }

    Row {
        anchors.fill: parent

        SplitView {
            height: parent.height
            width: parent.width-rightbar.width
            Layout.alignment: Qt.AlignTop
            orientation: Qt.Horizontal

            EditorPane {
                id: editor
                Layout.fillHeight: true
                width: 500
                simulator: root.simulator
                visualizer: root.visualizer
                Component.onCompleted: {
                    simulator.crashed.connect(editor.reportError)
                }
            }

            Item {
                id: visualizerItem
                Layout.alignment: Qt.AlignLeft
                Layout.fillHeight: true
                Layout.minimumWidth: 1
                AtomifyVisualizer {
                    id: visualizer
                    rootItem: visualizerItem
                    anchors.fill: parent
                    focus: true
                    focusMode: root.focusMode
                }

                WelcomeOverlay {
                    y: 200
                    visible: simulator.states.idle.active
                    anchors.horizontalCenter: parent.horizontalCenter
                    onNewScriptClicked: editor.editorWindow.newTab()
                    onExamplesClicked: rightbar.showExamples()
                }

                ControlBar {
                    id: controlBar1
                    simulator: root.simulator
                    visualizer: root.visualizer
                    visible: !root.focusMode
                    anchors.horizontalCenter: parent.horizontalCenter
                    y: parent.height - 100
                    width: 320
                    height: 64
                }
            }
        }

        RightBar {
            id: rightbar

            width: 300
            height: parent.height
            system: simulator.system
            visualizer: root.visualizer
        }
    }

    Item {
        id: shortcutes
        Shortcut {
            // Random placement here because it could not find the editor otherwise (Qt bug?)
            sequence: "Ctrl+R"
            context: Qt.ApplicationShortcut
            onActivated: {
                editor.editorWindow.runScript()
            }
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
}
