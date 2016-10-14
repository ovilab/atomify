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

Entity {
    id: root
    
    property vector3d size
    property var lights: [
        dummy
    ]

    function getScale(direction) {
        var dir = Qt.vector3d(0, 0, 0)
        dir[direction] = 1
        var added = Qt.vector3d(0.3, 0.3, 0.3)
        added[direction] = 0
        var result = size.times(dir).plus(added)
        console.log("Scale", dir, result)
        return result
    }
    
    function getTranslation(direction, index) {
        var a = -0.5 + index % 2
        var b = -0.5 + Math.floor(index / 2)
        var indexVector
        switch(direction) {
        case "x":
            indexVector = Qt.vector3d(0, a, b)
            break
        case "y":
            indexVector = Qt.vector3d(b, 0, a)
            break
        case "z":
            indexVector = Qt.vector3d(a, b, 0)
            break
        }
        var scaled = size.times(indexVector)
        console.log("Translation", indexVector, scaled)
        return scaled
    }
    
    NodeInstantiator {
        model: 4
        Entity {
            enabled: root.enabled
            components: [,
                outlineMaterial,
                mesh,
                transformX
            ]
            Transform {
                id: transformX
                translation: root.getTranslation("x", index)
                scale3D: root.getScale("x")
            }
        }
    }

    Light { id: dummy }
    
    NodeInstantiator {
        model: 4
        Entity {
            enabled: root.enabled
            components: [,
                outlineMaterial,
                mesh,
                transformY
            ]
            Transform {
                id: transformY
                translation: root.getTranslation("y", index)
                scale3D: root.getScale("y")
            }
        }
    }
    
    NodeInstantiator {
        model: 4
        Entity {
            enabled: root.enabled
            components: [,
                outlineMaterial,
                mesh,
                transformZ
            ]
            Transform {
                id: transformZ
                translation: root.getTranslation("z", index)
                scale3D: root.getScale("z")
            }
        }
    }

    CuboidMesh {id: mesh}

    ShaderBuilderMaterial {
        id: outlineMaterial
        fragmentColor: StandardMaterial {
            color: "white"
            lights: root.lights
        }
    }
}
