import SimVis 1.0
import ShaderNodes 1.0
import ShaderNodes 1.0 as Nodes

import Qt3D.Core 2.0
import Qt3D.Render 2.0

import QtQuick 2.0 as QQ2
import QtQuick.Scene3D 2.0

import SphereSimulator 1.0

Scene3D {
    aspects: ["render", "input", "logic"]
    Visualizer {
        id: visualizer
        SphereSimulator {
            id: simulator
            dt: 0.1
        }
        Spheres {
            id: spheresEntity
            camera: visualizer.camera
            sphereData: simulator.sphereData
        }
    }
}
