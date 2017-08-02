import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1
import Atomify 1.0

import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Input 2.0
import Qt3D.Extras 2.0

import QtQuick.Scene3D 2.0

import SimVis 1.0
import ShaderNodes 1.0

import "../desktop" // TODO should be separate controllers for desktop and mobile

Entity {
    id: root
    property var camera
    property var layer
    property alias color: material.fragmentColor
    property alias scale3D: transform.scale3D
    property vector3d translate
    property alias rotationX: transform.rotationX
    property alias rotationY: transform.rotationY
    property alias rotationZ: transform.rotationZ

    components: [
        mesh,
        material,
        transform,
        layer
    ]
    CylinderMesh {
        id: mesh
    }
    ShaderBuilderMaterial {
        id: material
        fragmentColor: "red"
    }
    Transform {
        id: transform
        translation: camera.viewCenter.plus(root.translate)
    }
}
