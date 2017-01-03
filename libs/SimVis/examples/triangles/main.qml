import QtQuick 2.5
import QtQuick.Window 2.2
import MySimulator 1.0
import SimVis 1.0
import SimVis.ShaderNodes 1.0

Window {
    visible: true
    width: 1024
    height: 720

    MySimulator {
        id: simulator
    }

    Visualizer {
        id: visualizer
        anchors.fill: parent
        simulator: simulator
        camera: camera
        backgroundColor: "black"

        Mesh {
            id: triangles
            fragmentColor: StandardMaterial {
                position: triangles.fragmentShader.position
                color: "blue"
                normal: triangles.fragmentShader.normal
                lights: ShaderGroup {
                    Light {
                        position: camera.position
                    }
                }
            }
        }

        TrackballNavigator {
            id: navigator
            anchors.fill: parent
            camera: camera
        }
    }

    Camera {
        id: camera
        farPlane: 1000000
        nearPlane: 0.1
    }
}

