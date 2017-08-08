import SimVis 1.0
import ShaderNodes 1.0
import ShaderNodes 1.0 as Nodes

import Qt3D.Core 2.0
import Qt3D.Render 2.0

import QtQuick 2.0 as QQ2
import QtQuick.Scene3D 2.0

import NeuroML 1.0

Scene3D {
    focus: true
    aspects: ["render", "logic", "input"]
    cameraAspectRatioMode: Scene3D.AutomaticAspectRatio

    Visualizer {
        id: visualizer

        NeuronReader {
            id: simulator
            source: "L5PC.cell.nml"
        }

        Cylinders {
            id: cylinders
            cylinderData: simulator.cylinderData
            fragmentColor: StandardMaterial {
                color: ImageTexture {
                    imageSource: "diffuse.webp"
                    vector: Multiply {
                        value1: 2.0
                        value2: cylinders.fragmentBuilder.position
                    }
                }
                normal: NormalMap {
                    color: ImageTexture {
                        imageSource: "normal.webp"
                        vector: Multiply {
                            value1: 2.0
                            value2: cylinders.fragmentBuilder.position
                        }
                    }
                }

                specularIntensity: 10.0
                hardness: 0.2
                lights: [light1, light2, light3, light4]
                Nodes.Light {
                    id: light1
                    position: Qt.vector3d(20.0, 0.0, -10.0)
                    attenuation: 0.1
                }
                Nodes.Light {
                    id: light2
                    position: Qt.vector3d(-20.0, 0.0, 10.0)
                    attenuation: 0.1
                }
                Nodes.Light {
                    id: light3
                    position: Qt.vector3d(-50.0, 10.0, 50.0)
                    attenuation: 0.1
                }
                Nodes.Light {
                    id: light4
                    position: Qt.vector3d(50.0, -20.0, -50.0)
                    attenuation: 0.1
                }
            }
        }
    }
}
