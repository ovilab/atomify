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

    property alias simulator: visualizer.simulator
    property Simulation simulation: null
    property string previousState: ""

    function loadSimulation(simulation) {
        if(!simulator.scriptHandler) {
            console.warn("WARNING: Cannot laod simulation because simulator has no scriptHandler.")
            return
        }

        simulator.willReset = true
        simulator.scriptHandler.reset()
        simulator.scriptHandler.runFile(simulation.scriptSource)
        mobileRoot.simulation = simulation
        simulator.clearSimulatorControls()

//        for(var i in simulation.controllers) {
//            var controller = simulation.controllers[i]
//            for(var j in controller.simulatorControls) {
//                var control = controller.simulatorControls[j]
//                control.parent = simulator
//                console.log("Created " + control + " on simulator " + simulator)
//            }
//        }

        mobileRoot.state = ""
    }

    onStateChanged: {
        dashboard.revealed = false
    }

    SimulationLoader {
        id: initialSimulationLoader
        folder: "qrc:/simulations/diffusion/simple_diffusion"
//        folder: "qrc:/simulations/silica/nanoporous"
        onLoaded: {
            loadSimulation(item)
        }
    }

    AtomifyVisualizer {
        id: visualizer
        anchors.fill: parent
    }

    RevealDashboardButton {
        id: revealDashboardButton
        revealed: mobileRoot.state == ""
        onClicked: {
            mainMenu.revealed = false
            dashboard.revealed = true
        }
    }

    RevealSimulationsViewButton {
        id: revealSimulationsViewButton
        revealed: mobileRoot.state == ""
        onClicked: {
            mainMenu.revealed = true
            dashboard.revealed = false
        }
    }

    DashboardRight {
        id: dashboard
        visualizer: visualizer
        simulator: mobileRoot.simulator
        simulation: mobileRoot.simulation
        onControlClicked: {
            mobileRoot.previousState = mobileRoot.state
            mobileRoot.state = "tools"
            console.log("Index was " + index)
            controlView.gotoIndex(index)
        }
    }

    DashboardControlView {
        id: controlView
        visualizer: visualizer
        simulator: mobileRoot.simulator
        simulation: mobileRoot.simulation
        anchors {
             bottom: visualizer.top
        }
        onBackClicked: {
            mobileRoot.state = mobileRoot.previousState
            dashboard.revealed = true
        }
    }

    SimulationsView {
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
            mobileRoot.previousState = mobileRoot.state
            mobileRoot.state = ""
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

