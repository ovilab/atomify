import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.1
import QtQuick.Layouts 1.3
import Qt.labs.settings 1.0
import Atomify 1.0
import "../../visualization"

Page {
    id: root
    property System system
    property AtomifyVisualizer visualizer
    property AtomifySimulator simulator
    focusPolicy: Qt.NoFocus

    onSimulatorChanged: {
        if(simulator) {
            simulator.rightBarFooterTextChanged.connect(function() {
                footerText.text = simulator.rightBarFooterText
            })
        }
    }

    function showExamples() {
        tabBar.currentIndex = 2
    }

    StackLayout {
        id: swipeView
        anchors.fill: parent
        currentIndex: tabBar.currentIndex
        clip: true

        SimulationSummary {
            width: swipeView.width
            height: swipeView.height
            // contentHeight: 2*height
            system: root.system
            visualizer: root.visualizer
            simulator: root.simulator
        }

        Rendering {
            width: swipeView.width
            height: swipeView.height
            visualizer: root.visualizer
        }
    }

    header: TabBar {
        id: tabBar
        currentIndex: swipeView.currentIndex
        onCurrentIndexChanged: {
            visualizer.focus = true // TODO: must be a better way?
        }

        TabButton {
            text: "Simulation"
            font.pixelSize: 12
        }
        TabButton {
            text: "Rendering"
            font.pixelSize: 12
        }
    }

    footer: Item {
        id: footer
        height: 150
        width: parent.width
        Rectangle {
            id: rect
            height: 2
            color: Material.accent
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: 5
        }

        Text {
            id: footerText
            anchors.left: parent.left
            anchors.bottom: parent.bottom
            anchors.top: rect.bottom
            width: parent.width*0.9

            anchors.margins: 10
            textFormat: TextEdit.RichText
            font.family: "DejaVu Sans Mono"
            font.pixelSize: 12
            wrapMode: Text.WordWrap
            text: "
<style>
h2 { text-align: center; }
a { font-weight: bold; color: #56b1b4; text-decoration: none; }
p { color: white; }
</style>
<p>Atomify is developed by Anders Hafreager and Svenn-Arne Dragly, University of Oslo.</p>
<p>Please report bugs <a href=\"reportBug\">here</a>.</p>
"
            onLinkActivated: {
                if(link==="reportBug") {
                    Qt.openUrlExternally("https://github.com/ovilab/atomify-lammps/issues")
                }

                if(link==="newVersion") {
                    Qt.openUrlExternally("https://github.com/ovilab/atomify/releases")
                }
            }
        }

        Image {
            anchors.top: parent.top
            anchors.right: parent.right
            source: "qrc:/images/close_white.png"
            width: parent.width*0.1
            height: width

            MouseArea {
                anchors.fill: parent
                onClicked: {
                   footer.height = 0
                    footer.visible = false
                }
            }
        }
    }
}
