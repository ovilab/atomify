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
            atomStyle: AtomStyle {
                id: myAtomStyle
            }
            scriptHandler: ScriptHandler {
                atomStyle: myAtomStyle
            }

//            RDF {
//                id: rdf
//                identifier: "rdf"
//                atomPairs: [1, 1, 1, 2, 2, 2]
//                numberOfBins: 100
//                group: "all"
//            }

        }

        Cylinders {
            cylinderData: simulator.cylinderData
            fragmentColor: StandardMaterial {
                lights: ShaderGroup {
                    Light {
                        position: visualizer.camera.position
                    }
                }
            }
        }

        Spheres {
            id: spheres
            camera: visualizer.camera
            sphereData: simulator.sphereData

//            PeriodicCopies {
//                id: periodicCopies
//                systemSize: simulator.systemSize
//                enabled: addPeriodicCopies
//            }

//            Slice {
//                id: mySlice
//                systemSize: simulator.systemSize
//                origo: Qt.vector3d(0,0,0)
//                distance: 0
//                normal: Qt.vector3d(1.0, 0.0, 0.0)
//                width: 5
//                enabled: false
//            }

//            SkyBoxReflection {
//                id: reflection
//                skybox: skybox
//                reflectivity: 0.2
//            }
        }

//        CylinderMesh {
//            id: mesh
//            radius: 1
//            length: 3
//            rings: 100
//            slices: 20
//        }

//        Transform {
//            id: transform
//            scale: 1.5
//            rotation: fromAxisAndAngle(Qt.vector3d(1, 0, 0), 45)
//        }

//        Material {
//            id: material
//            effect: Effect {}
//        }

//        Entity {
//            id: mainEntity
//            objectName: "mainEntity"
//            components: [ mesh, material, transform ]
//        }


    }
}
