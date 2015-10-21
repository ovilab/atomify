import QtQuick 2.5
import Atomify 1.0
import SimVis 1.0

Item {
    id: atomifyVisualizerRoot
    property Visualizer visualizer: visualizer
    property AtomifySimulator simulator
    property real scale: 0.23
    property Light light: light
    property Slice slice: mySlice
    property Camera camera: camera
    property bool addPeriodicCopies: false

    Visualizer {
        property Spheres spheres: spheres
        id: visualizer
        width: parent.width
        height: parent.height
        simulator: atomifyVisualizerRoot.simulator
        camera: camera
        backgroundColor: "#111"
        navigator: navigator
        onTouched: {
            atomifyVisualizerRoot.focus = true
        }

        TrackballNavigator {
            id: navigator
            anchors.fill: parent
            camera: camera
        }

//        Cylinders {
//            id: cylinders
//            visible: true
//            radius: 0.5
//            Light {
//                id: light2
//                ambientColor: spheres.color
//                specularColor: "white"
//                diffuseColor: spheres.color
//                ambientIntensity: 0.2
//                diffuseIntensity: 0.5
//                specularIntensity: 1.0
//                shininess: 40.0
//                attenuation: 0.001
//                position: camera.position
//            }
//        }

        Spheres {
            id: spheres
            visible: true
            scale: atomifyVisualizerRoot.scale
            color: "white"

            Light {
                id: light
                ambientColor: spheres.color
                specularColor: "white"
                diffuseColor: spheres.color
                ambientIntensity: 0.4
                diffuseIntensity: 0.8
                specularIntensity: 1.0
                specular: false
                shininess: 40.0
                attenuation: 0.001
                position: camera.position
            }

            PeriodicCopies {
                id: periodicCopies
                systemSize: simulator.systemSize
                enabled: addPeriodicCopies
            }

            Slice {
                id: mySlice
                systemSize: simulator.systemSize
                origo: Qt.vector3d(0,0,0)
                distance: 0
                normal: Qt.vector3d(1.0, 0.0, 0.0)
                width: 5
                enabled: false
            }
        }
    }


    Camera {
        id: camera
        nearPlane: 0.1
        farPlane: 1000
    }
}
