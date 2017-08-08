import Qt3D.Core 2.0
import Qt3D.Extras 2.0
import Qt3D.Render 2.0

import QtQuick 2.5 as QQ2
import QtQuick.Scene3D 2.0

import SimVis 1.0
import ShaderNodes 1.0
import ShaderNodes 1.0 as Nodes

import BondSimulator 1.0

Scene3D {
    aspects: "input"
    property alias simulator: simulator

    function doSomething() {
        if(transform.translation.x == 4.0) {
            transform.translation = Qt.vector3d(0.0, 0.0, 0.0)
        } else {
            transform.translation = Qt.vector3d(4.0, 0.0, 0.0)
        }
    }

    Visualizer {
        id: visualizer
        clearColor: "#dde"
        BondSimulator {
            id: simulator
            dt: 0.01
        }

        Spheres {
            id: spheresEntity
            camera: visualizer.camera
            sphereData: simulator.sphereData
        }

        Bonds {
            bondData: simulator.bondData
        }

        SphereMesh {
            id: mesh
            radius: 0.5
            rings: 300
            slices: 100
        }

        Transform {
            id: transform
            scale: 1
            rotation: fromAxisAndAngle(Qt.vector3d(1, 0, 0), 45)
        }

        PhongMaterial {
            id: material
            diffuse: "green"
        }

        Entity {
            id: mainEntity
            objectName: "mainEntity"
            components: [ mesh, material, transform ]
        }
    }
}
