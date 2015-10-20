import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Window 2.0
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0
import AtomifySimulator 1.0
import SimVis 1.0
import Compute 1.0
import LammpsOutput 1.0
import AtomStyle 1.0
import Qt.labs.settings 1.0

import "../visualization"
import "main-menu"
import "dashboard"
import "style"

Item {
    id: mobileRoot

    property AtomifySimulator simulator: null

    AtomifyVisualizer {
        simulator: mobileRoot.simulator
        anchors.fill: parent
        scale: 0.8
    }

    Item {
        id: revealDashboardButton
        property bool revealed: !dashboard.revealed && !mainMenu.revealed
        anchors {
            right: parent.right
            bottom: parent.bottom
        }
        enabled: revealed
        state: revealed ? "revealed" : "hidden"
        width: Style.touchableSize * 2.5
        height: width

        states: [
            State {
                name: "hidden"
                PropertyChanges {
                    target: revealDashboardButton
                    opacity: 0.0
                }
            },
            State {
                name: "revealed"
                PropertyChanges {
                    target: revealDashboardButton
                    opacity: 1.0
                }
            }
        ]

        transitions: [
            Transition {
                NumberAnimation {
                    properties: "opacity"
                    duration: 200
                    easing.type: Easing.InOutQuad
                }
            }
        ]

        Image {
            anchors {
                fill: parent
                margins: mobileRoot.width * 0.01
            }
            smooth: true

            source: "qrc:/images/dashboard.png"
        }

        MouseArea {
            anchors.fill: parent
            onPressed: {
                mainMenu.revealed = false
                dashboard.revealed = true
            }
        }
    }

    Item {
        id: revealSimulationsViewButton
        property bool revealed: !dashboard.revealed && !mainMenu.revealed

        anchors {
            top: mobileRoot.top
            left: mobileRoot.left
        }
        width: Style.touchableSize * 2.5
        height: width

        enabled: revealed
        state: revealed ? "revealed" : "hidden"

        states: [
            State {
                name: "hidden"
                PropertyChanges {
                    target: revealSimulationsViewButton
                    opacity: 0.0
                }
            },
            State {
                name: "revealed"
                PropertyChanges {
                    target: revealSimulationsViewButton
                    opacity: 1.0
                }
            }
        ]

        transitions: [
            Transition {
                NumberAnimation {
                    properties: "opacity"
                    duration: 200
                    easing.type: Easing.InOutQuad
                }
            }
        ]

        Image {
            anchors {
                fill: parent
                margins: parent.width * 0.2
            }
            source: "qrc:/images/systems.png"
        }

        MouseArea {
            anchors.fill: parent
            onPressed: {
                mainMenu.revealed = true
            }
        }
    }

    Dashboard {
        id: dashboard
        property bool isSettingSystemSize: false

//        temperature: molecularDynamics.temperature
//        pressure: molecularDynamics.pressure
//        kineticEnergy: molecularDynamics.kineticEnergy
//        potentialEnergy: molecularDynamics.potentialEnergy
//        totalEnergy: molecularDynamics.totalEnergy
//        time: molecularDynamics.time

        function updateSystemSize() {
//            if(molecularDynamics.isSettingSystemSize) {
//                return
//            }
//            isSettingSystemSize = true
//            molecularDynamics.systemSize = Qt.vector3d(systemSizeX, systemSizeY, systemSizeZ)
//            isSettingSystemSize = false
        }

        onSystemSizeXChanged: {
            updateSystemSize()
        }

        onSystemSizeYChanged: {
            updateSystemSize()
        }

        onSystemSizeZChanged: {
            updateSystemSize()
        }
    }

    MainMenu {
        id: mainMenu

        property bool wasPausedByReveal: false

        anchors.fill: parent

        Component.onCompleted: {
            loadFirstSimulation()
            revealed = true
        }

        onLoadSimulation: {
            simulator.willReset = true
            simulator.scriptHandler.reset()
            simulator.scriptHandler.runFile(simulation.scriptSource)
            revealed = false
        }

        onRevealedChanged: {
            if(revealed) {
                if(dashboard.running) {
                    wasPausedByReveal = true
                    dashboard.running = false
                }
            } else if(wasPausedByReveal) {
                dashboard.running = true
                wasPausedByReveal = false
            }
        }
    }
}

