import QtQuick 2.2
import QtQuick.Controls 1.1
import MySimulator 1.0
import SimVis 1.0
import SimVis.ShaderNodes 1.0

ApplicationWindow {
    id: applicationRoot
    visible: true
    width: 1600
    height: 900
    title: qsTr("Marching cubes demo")

    MySimulator {
        id: simulator
        marchingCubes: MarchingCubes {
            threshold: thresholdSlider.value
            resolution: Qt.vector3d(64,64,64)
            minValues: Qt.vector3d(-20, -20, -20)
            maxValues: Qt.vector3d(20, 20, 20)
        }
    }

    Visualizer {
        id: visualizer
        anchors.fill: parent
        simulator: simulator
        camera: camera
        backgroundColor: "gray"
        navigator: navigator
        TrackballNavigator {
            id: navigator
            anchors.fill: parent
            camera: camera
        }

        Camera {
            id: camera
            farPlane: 1000000
            nearPlane: 0.1
        }

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
    }

    Slider {
        id: thresholdSlider
        minimumValue: -2
        maximumValue: 2
        value: 0.0
    }
}
