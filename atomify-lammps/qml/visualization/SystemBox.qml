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
    property var layer
    property bool showSurfaces: true
    property real outlineAlpha: 0.4
    property real sideAlpha: 0.2
    property vector3d origin
    property vector3d size
    property vector3d sizePlus: size.plus(Qt.vector3d(1, 1, 1))
    property var lights: [
        dummy
    ]

    function getScale(direction) {
        var dir = Qt.vector3d(0, 0, 0)
        dir[direction] = 1
        var added = Qt.vector3d(0.3, 0.3, 0.3)
        added[direction] = 0
        var result = sizePlus.times(dir).plus(added)
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
        var scaled = sizePlus.times(indexVector)
        return scaled
    }
    
    NodeInstantiator {
        model: 4
        Entity {
            enabled: root.enabled
            components: [,
                outlineMaterial,
                mesh,
                transformX,
                layer
            ]
            Transform {
                id: transformX
                translation: root.getTranslation("x", index).plus(root.size.times(0.5).plus(root.origin))
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
                transformY,
                layer
            ]
            Transform {
                id: transformY
                translation: root.getTranslation("y", index).plus(root.size.times(0.5).plus(root.origin))
                scale3D: root.getScale("y")
            }
            Transform {
                id: transformYOrigin
                translation: origin
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
                transformZ,
                layer
            ]
            Transform {
                id: transformZ
                translation: root.getTranslation("z", index).plus(root.size.times(0.5).plus(root.origin))
                scale3D: root.getScale("z")
            }
        }
    }

    CuboidMesh {id: mesh}

    ShaderBuilderMaterial {
        id: outlineMaterial
        fragmentColor: StandardMaterial {
            color: Qt.rgba(0.6, 0.8, 1, outlineAlpha)
            lights: root.lights
        }
    }

    Entity {
        enabled: root.enabled && root.showSurfaces
        components: [
            mesh,
            layer,
            surfaceMaterial,
            surfaceTransform
        ]

        Transform {
            id: surfaceTransform
            translation: root.size.times(0.5).plus(root.origin)
            scale3D: root.sizePlus
        }
        Timer {
            id: timer
            property real time
            property real previousTime: 0
            running: true
            repeat: true
            interval: 16
            onTriggered: {
                if(previousTime === 0) {
                    previousTime = Date.now()
                    return
                }

                time += Date.now() - previousTime
                previousTime = Date.now()
            }
        }

        ShaderBuilderMaterial {
            id: surfaceMaterial
            fragmentColor: StandardMaterial {
                color: Qt.rgba(1, 1, 1, sideAlpha)
                lights: root.lights
            }
            deferredColor: Qt.rgba(1, 1, 1, sideAlpha)
        }
    }

}
