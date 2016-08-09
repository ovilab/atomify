import QtQuick 2.7
import QtQuick.Controls 2.0
import Atomify 1.0
import "../visualization"

Page {
    id: root
    property System system
    property AtomifyVisualizer visualizer

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
        }
    }

    header: TabBar {
        id: tabBar
        currentIndex: swipeView.currentIndex

        TabButton {
            text: "Simulation"
        }
        TabButton {
            text: "Rendering"
        }
    }
}
