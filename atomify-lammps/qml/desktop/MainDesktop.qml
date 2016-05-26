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

    Row {
        anchors.fill: parent

        SplitView {
            height: parent.height
            width: parent.width-300
            Layout.alignment: Qt.AlignTop
            orientation: Qt.Horizontal

            AtomifyTabView {
                Layout.fillHeight: true
                simulator: simulator
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
            width: 300
            height: parent.height
            system: simulator.system ? simulator.system : null
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
