import QtQuick 2.4
import MySimulator 1.0
import SimVis 1.0
import LammpsOutput 1.0

Item {
    id: atomifyVisualizerRoot
    property Visualizer visualizer: visualizer
    property MySimulator simulator
    property Light light: light

    Visualizer {
        property Spheres spheres: spheres
        id: visualizer
        width: parent.width
        height: parent.height
        simulator: atomifyVisualizerRoot.simulator
        camera: camera
        backgroundColor: "#111"
        navigator: navigator

        TrackballNavigator {
            id: navigator
            anchors.fill: parent
            camera: camera
        }

        Spheres {
            id: spheres
            visible: true
            scale: 0.23
            color: "white"

            Light {
                id: light
                ambientColor: spheres.color
                specularColor: "white"
                diffuseColor: spheres.color
                ambientIntensity: 0.2
                diffuseIntensity: 0.5
                specularIntensity: 1.0
                shininess: 40.0
                attenuation: 0.001
                position: camera.position
            }
        }
    }

    Rectangle {
        x: 20
        y: 20
        width: 200
        height: 80
        radius: 4
        color: Qt.rgba(1.0, 1.0, 1.0, 0.75)
        Text {
            x: 10
            y: 10
            font.bold: true
            text: "Number of atoms: "+simulator.numberOfAtoms
        }
        Text {
            x: 10
            y: 30
            font.bold: true
            text: "Number of atom types: "+simulator.numberOfAtomTypes
        }

        Text {
            x: 10
            y: 50
            font.bold: true
            text: "System size: ["+simulator.systemSize.x.toFixed(1)+","+simulator.systemSize.y.toFixed(1)+","+simulator.systemSize.z.toFixed(1)+"]"
        }
    }

    Camera {
        id: camera
        nearPlane: 0.1
        farPlane: 1000
    }
}
