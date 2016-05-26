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
    property bool focusMode: false
    onSimulatorChanged: {
        console.log("Simulator here: ", simulator)
    }

    Row {
        anchors.fill: parent

        SplitView {
            height: parent.height
            width: parent.width-simulationSummary.width
            Layout.alignment: Qt.AlignTop
            orientation: Qt.Horizontal

            AtomifyTabView {
                id: tabView
                Layout.fillHeight: true
                simulator: simulator
                width: 500

                onEditorTabChanged: {
                    console.log("Editor changed. Simulator: ", simulator)
                    if(simulator && tabView.editorTab) {
                        simulator.errorInLammpsScript.connect(tabView.editorTab.editorTab.reportError)
                    }
                }
            }

            AtomifyVisualizer {
                id: visualizer
                focus: true
                Layout.alignment: Qt.AlignLeft
                Layout.fillHeight: true
                Layout.minimumWidth: 1
            }
        }

        SimulationSummary {
            id: simulationSummary
            width: 300
            height: parent.height
            system: simulator.system ? simulator.system : null
        }
    }

    function toggleFocusMode() {
        if(focusMode) {
            simulationSummary.width = 300
            simulationSummary.visible = true
            tabView.visible = true
            focusMode = false
            tabDisable.hideTabDisable.start()
        } else {
            simulationSummary.width = 0
            tabView.visible = false
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
            onActivated: tabView.editorTab.runScript()
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

    Rectangle {
        id: tabDisable
        property bool isVisible: false
        property alias showTabDisable: show
        property alias hideTabDisable: hide
        x: parent.width*0.5 - 0.5*width
        y: 10
        width: tabDisableLabel.width + 16
        height: tabDisableLabel.height + 10
        opacity: 0
        radius: 5

        NumberAnimation on opacity {
            id: hide
            to: 0; duration: 200
        }

        NumberAnimation on opacity {
            id: show
            to: 0.8; duration: 200
        }

        Label {
            id: tabDisableLabel
            x: 8
            y: 5
            text: "Press Tab to disable focus mode (click to hide)"
        }
        MouseArea {
            anchors.fill: parent
            onClicked: {
                hide.start()
            }
        }
    }
}
