import QtQuick 2.2
import MySimulator 1.0
import SimVis 1.0
import LammpsOutput 1.0

Item {
    id: atomifyVisualizerRoot
    property Visualizer visualizer: visualizer
    property MySimulator simulator


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
                ambientIntensity: 0.025
                diffuseIntensity: 0.5
                specularIntensity: 1.0
                shininess: 40.0
                attenuation: 0.01
                position: camera.position
            }
        }
    }

    Camera {
        id: camera
        nearPlane: 0.1
    }
}
