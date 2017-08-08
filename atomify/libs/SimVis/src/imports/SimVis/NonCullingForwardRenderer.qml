import Qt3D.Core 2.0
import Qt3D.Render 2.0

TechniqueFilter {
    // Expose camera to allow user to choose which camera to use for rendering
    property alias camera: cameraSelector.camera
    property alias clearColor: clearBuffers.clearColor
    property alias viewportRect: viewport.normalizedRect
    property alias window: surfaceSelector.surface

    // Select the forward rendering Technique of any used Effect
    matchAll: [ FilterKey { name: "renderingStyle"; value: "forward" } ]

    RenderSurfaceSelector {
        id: surfaceSelector
        Viewport {
            id: viewport
            normalizedRect: Qt.rect(0.0, 0.0, 1.0, 1.0)

            // Use the specified camera
            CameraSelector {
                id : cameraSelector
                FrustumCulling {
                    enabled: false
                    ClearBuffers {
                        id: clearBuffers
                        buffers : ClearBuffers.ColorDepthBuffer
                        clearColor: "white"
                    }
                }
            }
        }
    }
}
