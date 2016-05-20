import QtQuick 2.5
import QtQuick.Controls 1.4
import Atomify 1.0

import Qt3D.Core 2.0
import Qt3D.Render 2.0

import QtQuick.Scene3D 2.0

import SimVis 1.0
import SimVis.ShaderNodes 1.0

Scene3D {
    id: atomifyVisualizerRoot
    property alias visualizer: visualizer
    property alias simulator: mySimulator
    // property alias rdf: rdf
    property real scale: 0.23
    property bool addPeriodicCopies: false

    aspects: ["render", "input"]

    Visualizer {
        id: visualizer
        property Spheres spheres: spheres

        clearColor: "#012"
        camera.aspectRatio: atomifyVisualizerRoot.width / atomifyVisualizerRoot.height

        AtomifySimulator {
            id: mySimulator
            simulationSpeed: 1
        }

        Spheres {
            id: spheres
            camera: visualizer.camera
            sphereData: simulator.atoms.sphereData
        }

        Bonds {
            id: bonds
            bondData: simulator.atoms.bondData
        }
    }
}
