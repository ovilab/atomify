import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Window 2.0
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0
import Atomify 1.0
import SimVis 1.0
import Qt.labs.settings 1.0

import "qrc:/core"
import "qrc:/mobile/main-menu"
import "qrc:/mobile/dashboard"
import "qrc:/mobile/style"
import "qrc:/mobile/hud"
import "qrc:/mobile/simulations"
import "qrc:/visualization"

Item {
    id: mobileRoot

    property AtomifySimulator simulator: null
    property Simulation simulation: null
    property string previousState: ""

    function loadSimulation(simulation) {
        if(!simulator.scriptHandler) {
            return
        }

        simulator.willReset = true
        simulator.scriptHandler.reset()
        simulator.scriptHandler.runFile(simulation.scriptSource)
        mobileRoot.simulation = simulation
        mobileRoot.state = ""
    }

    onStateChanged: {
        dashboard.revealed = false
    }

    SimulationLoader {
        id: initialSimulationLoader
        folder: "qrc:/simulations/diffusion/simple_diffusion"
        onLoaded: {
            loadSimulation(item)
        }
    }

    AtomifyVisualizer {
        id: visualizer
        simulator: mobileRoot.simulator
        width: parent.width
        height: parent.height
        scale: 0.8
    }

    RevealDashboardButton {
        id: revealDashboardButton
        revealed: mobileRoot.state == ""
    }

    RevealSimulationsViewButton {
        id: revealSimulationsViewButton
        revealed: mobileRoot.state == ""
        onClicked: {
            mainMenu.revealed = true
        }
    }

    DashboardRight {
        id: dashboard
        simulation: mobileRoot.simulation
    }

    SimulationsViewNew {
        id: simulationsView
        width: parent.width
        height: parent.height
        anchors {
            top: visualizer.bottom
        }
        onSimulationClicked: {
            singleSimulation.simulation = simulation
            mobileRoot.previousState = mobileRoot.state
            mobileRoot.state = "singleSimulation"
        }
        onBackClicked: {
            mainMenu.revealed = true
        }
    }

    SingleSimulationView {
        id: singleSimulation
        width: parent.width
        height: parent.height
        anchors {
            top: simulationsView.bottom
        }
        onPlayClicked: {
            loadSimulation(simulation)
        }
        onBackClicked: {
            mobileRoot.state = mobileRoot.previousState
        }
    }

    MainMenuLeft {
        id: mainMenu

        simulation: mobileRoot.simulation
        onContinueClicked: {
            mobileRoot.state = ""
        }
        onSimulationsClicked: {
            mobileRoot.state = "simulations"
        }
        onSimulationClicked: {
            singleSimulation.simulation = simulation
            mainMenu.revealed = false
            mobileRoot.previousState = mobileRoot.state
            mobileRoot.state = "singleSimulation"
        }
    }

    states: [
        State {
            name: "simulations"
            PropertyChanges {
                target: visualizer
                y: -visualizer.height
            }
        },
        State {
            name: "tools"
            PropertyChanges {
                target: visualizer
                y: visualizer.height
            }
        },
        State {
            name: "singleSimulation"
            PropertyChanges {
                target: visualizer
                y: -visualizer.height * 2
            }
        }
    ]

    transitions: [
        Transition {
            NumberAnimation {
                target: visualizer
                properties: "x,y"
                duration: 360
                easing.type: Easing.InOutQuad
            }
        }
    ]
}

