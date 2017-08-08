import QtQuick.Scene3D 2.0

import Qt3D.Core 2.0
import Qt3D.Extras 2.0
import Qt3D.Input 2.0
import Qt3D.Logic 2.0
import Qt3D.Render 2.0

import ShaderNodes 1.0

Scene3D {
    id: root

    property alias material: material
    property alias source: mesh.source

    aspects: ["input", "logic"]

    Entity {
        id: rootEntity

        components: [
            RenderSettings {
                activeFrameGraph: ForwardRenderer {
                    camera: Camera {
                        id: mainCamera
                        projectionType: CameraLens.PerspectiveProjection
                        fieldOfView: 50
                        aspectRatio: root.width / root.height
                        nearPlane: 1.0
                        farPlane: 10000.0
                        position: Qt.vector3d(16, 8, 16)
                        viewCenter: Qt.vector3d(0, 0, 0)
                        upVector: Qt.vector3d(0.0, 1.0, 0.0)
                    }
                    clearColor: Qt.rgba(0.2, 0.2, 0.2)
                }
            },
            InputSettings {}
        ]

        MouseDevice {
            id: mouseDevice
        }

        MouseHandler {
            id: mouseHandler

            sourceDevice: mouseDevice
            onWheel: {
                var factor = wheel.angleDelta.y * 0.003
                var offset = mainCamera.viewVector.times(factor)
                mainCamera.translateWorld(offset, Camera.DontTranslateViewCenter)
            }
        }

        Mesh {
            id: mesh
            onSourceChanged: {
                torusEntity.parent = null
            }
        }

        ShaderBuilderMaterial {
            id: material
        }

        TorusMesh {
            id: sphereMesh
            minorRadius: 2.0
            radius: 8.0
            rings: 100
            slices: 20
        }

        Entity {
            id: torusEntity
            components: [
                sphereMesh,
                material
            ]
        }

        Entity {
            components: [
                mesh,
                material
            ]
        }

        OrbitCameraController {
            camera: mainCamera
        }
    }
}
