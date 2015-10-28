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
import "main-menu"
import "dashboard"
import "style"
import "hud"
import "simulations"

Item {
    id: mobileRoot

    property AtomifySimulator simulator: null

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

    AtomifyVisualizer {
        id: visualizer
        simulator: mobileRoot.simulator
        width: parent.width
        height: parent.height
        scale: 0.8

        MouseArea {
            anchors.fill: parent
            onClicked: {
                mainMenu.revealed = false
                dashboard.revealed = false
            }
        }

        MouseArea {
            anchors {
                top: parent.bottom
                left: parent.left
                right: parent.right
            }
            height: 100
            onClicked: {
                mobileRoot.state = ""
            }
        }

        MouseArea {
            anchors {
                bottom: parent.top
                left: parent.left
                right: parent.right
            }
            height: 100
            onClicked: {
                mobileRoot.state = ""
            }
        }

        MouseArea {
            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
            }
            height: 100
            onClicked: {
                console.log("Visualizer top")
                mobileRoot.state = "tools"
            }
        }

        MouseArea {
            anchors {
                left: parent.left
                right: parent.right
                bottom: parent.bottom
            }
            height: 100
            onClicked: {
                console.log("Simulations")
                mobileRoot.state = "simulations"
            }
        }
    }

    RevealDashboardButton {
        id: revealDashboardButton
        revealed: mobileRoot.state == ""
    }

    Dashboard {
        id: dashboard
    }


    SimulationsViewNew {
        id: simulationsView
        width: parent.width
        height: parent.height
        anchors {
            top: visualizer.bottom
        }
        onClickedSimulation: {
            singleSimulation.simulation = simulation
            mobileRoot.state = "singleSimulation"
        }
    }


    SingleSimulationView {
        id: singleSimulation
        width: parent.width
        height: parent.height
        anchors {
            top: simulationsView.bottom
        }
        onLoadSimulation: {
            if(!simulator.scriptHandler) {
                return
            }

            simulator.willReset = true
            simulator.scriptHandler.reset()
            simulator.scriptHandler.runFile(simulation.scriptSource)
            mobileRoot.state = ""
        }
    }

    RevealSimulationsViewButton {
        id: revealSimulationsViewButton
        revealed: mobileRoot.state == ""
        onClicked: {
            mainMenu.revealed = true
        }
    }

    MainMenuLeft {
        id: mainMenu
        onContinueClicked: {
            mobileRoot.state = ""
        }

        onSimulationsClicked: {
            mobileRoot.state = "simulations"
        }
    }
}

