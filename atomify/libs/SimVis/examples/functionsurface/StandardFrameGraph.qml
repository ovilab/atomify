import Qt3D.Core 2.0
import Qt3D.Render 2.0

import QtQuick 2.0 as QQ2
import QtQuick.Scene3D 2.0

import MySimulator 1.0

FrameGraph {
    id: frameGraph
    property Camera camera: Camera {
        projectionType: CameraLens.PerspectiveProjection
        fieldOfView: 50
        aspectRatio: 16/9
        nearPlane : 0.1
        farPlane : 10000.0
        position: Qt.vector3d(0.0, 0.0, 35.0)
        upVector: Qt.vector3d(0.0, 1.0, 0.0)
        viewCenter: Qt.vector3d(0.0, 0.0, 0.0)
    }
    activeFrameGraph: TechniqueFilter {
        requires: [
            Annotation {
                name: "renderingStyle"
                value: "forward"
            }
        ]
        Viewport {
            id: viewport
            rect: Qt.rect(0.0, 0.0, 1.0, 1.0)
            clearColor: "#afc"
            CameraSelector {
                id : cameraSelector
                camera: frameGraph.camera
                FrustumCulling {
                    enabled: false
                    ClearBuffer {
                        buffers : ClearBuffer.ColorDepthBuffer
                    }
                }
            }
        }
    }
}
