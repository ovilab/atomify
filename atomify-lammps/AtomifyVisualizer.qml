import QtQuick 2.2
import MySimulator 1.0
import SimVis 1.0
import LammpsOutput 1.0

Item {
    // property MySimulator simulator: simulator
    property MySimulator simulator: simulator
    property Visualizer visualizer: visualizer

    MySimulator {
        id: simulator
        lammpsOutput: output
        simulationSpeed: 1
        onNewCameraPositionChanged: {
            upVectorAnimation.from = camera.upVector
            positionAnimation.to = newCameraPosition
            positionAnimation.from = camera.position
            if(positionAnimation.running) {
                positionAnimation.stop()
            }
            if(upVectorAnimation.running) {
                upVectorAnimation.stop()
            }

            positionAnimation.start()
            upVectorAnimation.start()
        }
    }

    LammpsOutput {
        id: output
    }

    Visualizer {
        property Spheres spheres: spheres
        id: visualizer
        width: parent.width
        height: parent.height
        simulator: simulator
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

    PropertyAnimation {
        id: positionAnimation
        target: camera
        property: "position"
        from: camera.position
        to: simulator.newCameraPosition
        duration: 700
        easing.type: Easing.InOutQuad
    }

    PropertyAnimation {
        id: upVectorAnimation
        target: camera
        property: "upVector"
        from: camera.upVector
        to: Qt.vector3d(0, -1, 0)
        duration: 700
        easing.type: Easing.InOutQuad
    }
}
