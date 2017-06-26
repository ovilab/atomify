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
    property Simulation simulation: simulationLoader.item
    property alias visualizer: visualizer.visualizer
    property string previousState: ""
    property var simulationsModel: [
        {
            name: "LAMMPS Examples",
            simulations: [
                "qrc:/simulations/lammps/crack",
                "qrc:/simulations/lammps/flow_couette",
                "qrc:/simulations/lammps/flow_pois",
                "qrc:/simulations/lammps/friction",
                "qrc:/simulations/lammps/obstacle"
            ]
        },
        {
            name: "ReaxFF",
            simulations: [
                "qrc:/simulations/reaxff/CHO",
                "qrc:/simulations/reaxff/ssz_13",
                "qrc:/simulations/reaxff/VOH",
                "qrc:/simulations/reaxff/AB",
                "qrc:/simulations/reaxff/FeOH3",
                "qrc:/simulations/reaxff/RDX",
                "qrc:/simulations/reaxff/ZnOH2"
            ]
        },
        {
            name: "Other",
            simulations: [
                "qrc:/simulations/new/abstract_translocation",
                "qrc:/simulations/new/cmap",
                "qrc:/simulations/new/ice_crystal",
                "qrc:/simulations/silica/faceted_cylinder"
            ]
        },
        {
            name: "Water",
            simulations: [
                "qrc:/simulations/water/singlewater",
                "qrc:/simulations/water/vapor",
                "qrc:/simulations/water/nanotube"
            ]
        },
        {
            name: "Granular materials",
            simulations: [
                "qrc:/simulations/granular/patterns",
                "qrc:/simulations/granular/patterns_final",
                "qrc:/simulations/granular/maxwell"
            ]
        },
        {
            name: "Porous media",
            simulations: [
                "qrc:/simulations/silica/betacristobalite",
                "qrc:/simulations/silica/generate_nanoporous",
                "qrc:/simulations/silica/zeolite_zsm5"
            ]
        },
        {
            name: "Diffusion",
            simulations: [
                "qrc:/simulations/diffusion/simple_diffusion",
                "qrc:/simulations/diffusion/diffusion_coefficient"
            ]
        }
    ]

    focus: true

    onStateChanged: {
        dashboard.revealed = false
    }

    function loadSimulation(simulation) {
        simulationLoader.folder = simulation.folder
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
        id: simulationLoader
        folder: "qrc:/simulations/water/singlewater"
        onLoaded: {
            var simulation = item
            if(!simulator.scriptHandler) {
                console.warn("WARNING: Cannot laod simulation because simulator has no scriptHandler.")
                return
            }

            start()

            visualizer.animateCameraTo(simulation.initialCameraPosition, simulation.initialUpVector, simulation.initialViewCenter, 1000)
            simulation.system = simulator.system
            periodicImages.enabled = (simulation.periodicImagesX !== 1) || (simulation.periodicImagesY !== 1) || (simulation.periodicImagesZ !== 1)
            periodicImages.numberOfCopiesX = simulation.periodicImagesX
            periodicImages.numberOfCopiesY = simulation.periodicImagesY
            periodicImages.numberOfCopiesZ = simulation.periodicImagesZ
            visualizer.attenuation = simulation.lightAttenuation
            visualizer.camera.orthographic = simulation.orthographic
            visualizer.customColor.code = simulation.customColorShaderCode
            addSimulatorControls()

            mobileRoot.state = ""
        }

        function start() {
            simulator.willReset = true
            simulator.scriptHandler.reset()
            simulator.scriptHandler.runFile(simulation.scriptSource)
            addSimulatorControls()
        }

        function resetSimulatorControls() {
            for(var i in simulation.controllers) {
                var controller = simulation.controllers[i]
                for(var j in controller.simulatorControls) {
                    var control = controller.simulatorControls[j]
                    control.clear()
                }
            }
        }

        function addSimulatorControls() {
            simulator.clearSimulatorControls()
            for(var i in simulation.controllers) {
                var controller = simulation.controllers[i]
                for(var j in controller.simulatorControls) {
                    var control = controller.simulatorControls[j]
                    control.clear()
                    simulator.addSimulatorControl(control);
                }
            }
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
        revealed: mobileRoot.state == "" && !splashScreen.visible
        onClicked: {
            mainMenu.revealed = true
            dashboard.revealed = false
            informationPanel.revealed = false
        }
    }

    RevealDashboardButton {
        id: revealDashboardButton
        revealed: mobileRoot.state == "" && !splashScreen.visible && controlView.numControllers>0
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
        revealed: mobileRoot.state == "" && !splashScreen.visible
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
            // console.log("Index was " + index)
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
        active: mobileRoot.state === "tools"
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
        model: simulationsModel
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
        onRestartSimulation: {
            simulationLoader.start()
        }

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

    SearchView {
        id: searchView
        visible: false
        model: simulationsModel
        onLoadSimulation: {
            mobileRoot.loadSimulation(simulation)
        }
    }

    Shortcut {
        sequence: "Ctrl+K"
        onActivated: {
            searchView.visible = !searchView.visible
        }
    }

    Shortcut {
        sequence: Qt.Key_Escape
        onActivated: {
            searchView.visible = false
        }
    }

    Text {
        id: labelText
        visible:  !informationPanel.revealed && mobileRoot.state != "tools" && mobileRoot.state != "simulations"
        anchors {
            top: parent.top
            right: parent.right
            margins: Style.baseMargin * 0.5
        }
        color: "white"
        font.pixelSize: Style.font.size
        text: simulator.scriptHandler.label
    }

    Rectangle {
        anchors {
            top: labelText.bottom
            right: parent.right
            margins: Style.baseMargin * 0.5
        }
        width: Style.touchableSize * 3
        height: Style.touchableSize * 0.2
        visible: simulator.scriptHandler.label.length > 0 && labelText.visible
        color: "#aaaaaaaa"

        Rectangle {
            anchors {
                top: parent.top
                left: parent.left
                bottom: parent.bottom
                margins: 2
            }
            width: simulator.currentRunStep / simulator.runStepCount * (parent.width - anchors.margins * 2)
        }
    }

    SplashScreen {
        id: splashScreen
        anchors.fill: parent
    }

    Keys.onPressed: {
        if(event.key === Qt.Key_Back) {
            event.accepted = true
        }
        if(event.key === Qt.Key_Escape) {
            splashScreen.visible = false
            event.accepted = true
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

    Shortcut {
        sequence: "Space"
        onActivated: {
            mySimulator.paused = !mySimulator.paused
        }
    }

//    Slider {
//        minimumValue: -5
//        maximumValue: 0
//        value: -5
//        stepSize: 0.1
//        onValueChanged: {
//            // mySimulator.system.atoms.occlusionFactor = value
//            visualizer.attenuation = Math.pow(10,value)
//        }
//    }
}

