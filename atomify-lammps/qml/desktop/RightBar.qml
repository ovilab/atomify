import QtQuick 2.7
import QtQuick.Controls 2.0
import Atomify 1.0
import "../visualization"

Page {
    id: root
    property System system
    property AtomifyVisualizer visualizer
    focusPolicy: Qt.NoFocus

    SwipeView {
        id: swipeView
        anchors.fill: parent
        currentIndex: tabBar.currentIndex
        clip: true

        SimulationSummary {
            width: swipeView.width
            height: swipeView.height
            system: root.system
        }

        Rendering {
            width: swipeView.width
            height: swipeView.height
            visualizer: root.visualizer
        }

        Examples {

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
        TabButton {
            text: "Examples"
            font.pixelSize: 12
        }
    }
}
