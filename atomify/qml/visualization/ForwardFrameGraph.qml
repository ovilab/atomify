import QtQuick 2.5
import QtQuick.Controls 1.4
import Atomify 1.0

import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Input 2.0
import Qt3D.Extras 2.0

import QtQuick.Scene3D 2.0

import SimVis 1.0
import ShaderNodes 1.0

import "../desktop" // TODO should be separate controllers for desktop and mobile

Viewport {
    id: root

    property Camera camera: Camera {}
    property alias surface: surfaceSelector.surface
    property Layer atomLayer
    property Layer guideLayer
    property Layer outlineLayer
    property alias clearColor: clearBuffer.clearColor

    normalizedRect: Qt.rect(0.0, 0.0, 1.0, 1.0)

    TechniqueFilter {
        matchAll: FilterKey { name: "renderingStyle"; value: "forward" }
        RenderSurfaceSelector {
            id: surfaceSelector
            RenderPassFilter {
                CameraSelector {
                    camera: root.camera
                    LayerFilter {
                        layers: atomLayer
                        ClearBuffers {
                            id: clearBuffer
                            clearColor: "#000"
                            buffers: ClearBuffers.ColorDepthBuffer
                        }
                    }
                    LayerFilter {
                        layers: guideLayer
                    }
                    LayerFilter {
                        layers: outlineLayer
                    }
                }
            }
        }
    }
}
