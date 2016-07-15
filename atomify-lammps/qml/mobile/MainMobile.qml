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
    property alias simulator: mySimulator
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

        console.log("Will load simulation")
        visualizer.animateCameraTo(simulation.initialCameraPosition, simulation.initialUpVector, simulation.initialViewCenter, 1000)

        periodicImages.enabled = (simulation.periodicImagesX !== 1) || (simulation.periodicImagesY !== 1) || (simulation.periodicImagesZ !== 1)
        periodicImages.numberOfCopiesX = simulation.periodicImagesX
        periodicImages.numberOfCopiesY = simulation.periodicImagesY
        periodicImages.numberOfCopiesZ = simulation.periodicImagesZ

        simulator.clearSimulatorControls()

        for(var i in simulation.controllers) {
            var controller = simulation.controllers[i]
            for(var j in controller.simulatorControls) {
                var control = controller.simulatorControls[j]
                //                control.parent = simulator
                simulator.addSimulatorControl(control);
                console.log("Added", control, "to simulator", simulator)
            }
        }

        mobileRoot.state = ""
    }

    onStateChanged: {
        dashboard.revealed = false
    }

    AtomifySimulator {
        id: mySimulator

        running: mobileRoot.state === "" || mobileRoot.state === "tools"
        simulationSpeed: 1
        system.atoms.modifiers: [
            colorModifier,
            periodicImages
        ]

        PeriodicImages {
            id: periodicImages
            enabled: false
            numberOfCopiesX: 1
            numberOfCopiesY: 1
            numberOfCopiesZ: 1
        }

        ColorModifier {
            id: colorModifier
            scale: 0.25
        }
    }

    SimulationLoader {
        id: initialSimulationLoader
        // folder: "qrc:/simulations/diffusion/simple_diffusion"
        folder: "qrc:/simulations/water/singlewater"
        // folder: "qrc:/simulations/silica/silica"
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

        TrackballNavigator {
            id: navigator
            anchors.fill: parent
            camera: visualizer.camera
        }
    }

    RevealSimulationsViewButton {
        id: revealSimulationsViewButton
        revealed: mobileRoot.state == ""
        onClicked: {
            mainMenu.revealed = true
            dashboard.revealed = false
            informationPanel.revealed = false
        }
    }

    RevealDashboardButton {
        id: revealDashboardButton
        revealed: mobileRoot.state == ""
        onClicked: {
            mobileRoot.previousState = mobileRoot.state
            mobileRoot.state = "tools"
        }
    }

    AboutButton {
        anchors {
            bottom: revealDashboardButton.top
            right: parent.right
        }
        revealed: mobileRoot.state == ""
        onClicked: {
            informationPanel.revealed = true
            mainMenu.revealed = false
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

    InformationPanel {
        id: informationPanel
        simulation: mobileRoot.simulation
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
            //            dashboard.revealed = true
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

