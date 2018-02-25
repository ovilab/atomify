import QtQuick 2.5
import QtQuick.Controls 1.4
import Atomify 1.0

import Qt3D.Core 2.0
import Qt3D.Render 2.1
import Qt3D.Input 2.0
import Qt3D.Extras 2.0

import QtQuick.Scene3D 2.0

import SimVis 1.0
import ShaderNodes 1.0

import "../desktop" // TODO should be separate controllers for desktop and mobile

Viewport {
    id: root
    property real width: 10
    property real height: 10
    property Camera camera: Camera {}
    property alias surface: surfaceSelector.surface
    property Layer atomLayer
    property Layer guideLayer
    property Layer outlineLayer
    property alias renderCapture: renderCapture
    property alias clearColor: clearBuffer.clearColor
    property alias colorTexture: colorTexture
    property alias particleIdTexture: particleIdTexture

    normalizedRect: Qt.rect(0.0, 0.0, 1.0, 1.0)

    TechniqueFilter {
        matchAll: FilterKey { name: "renderingStyle"; value: "forward" }
        RenderSurfaceSelector {
            id: surfaceSelector
            RenderPassFilter {
                RenderTargetSelector {
                    target: RenderTarget {
                        attachments: [
                            RenderTargetOutput {
                                id: colorOutput
                                objectName : "colorOut"
                                attachmentPoint : RenderTargetOutput.Color0
                                texture : Texture2D {
                                    id : colorTexture
                                    width : root.width
                                    height : root.height
                                    format : Texture.RGBA32F
                                    generateMipMaps : false
                                    magnificationFilter : Texture.Nearest
                                    minificationFilter : Texture.Nearest
                                    wrapMode {
                                        x: WrapMode.ClampToEdge
                                        y: WrapMode.ClampToEdge
                                    }
                                }
                            },
                            RenderTargetOutput {
                                id: particleIdOutput
                                objectName: "particleIdOut"
                                attachmentPoint : RenderTargetOutput.Color1
                                texture: Texture2D {
                                    id: particleIdTexture
                                    width : root.width
                                    height : root.height
                                    format : Texture.R32F
                                    generateMipMaps : false
                                    magnificationFilter : Texture.Nearest
                                    minificationFilter : Texture.Nearest
                                    wrapMode {
                                        x: WrapMode.ClampToEdge
                                        y: WrapMode.ClampToEdge
                                    }
                                }
                            },
                            RenderTargetOutput {
                                id: depthOutput
                                objectName: "depth"
                                attachmentPoint: RenderTargetOutput.Depth
                                texture: Texture2D {
                                    id: depthTexture
                                    width : root.width
                                    height : root.height
                                    format: Texture.D32
                                    generateMipMaps: false
                                    magnificationFilter: Texture.Nearest
                                    minificationFilter: Texture.Nearest
                                    wrapMode {
                                        x: WrapMode.ClampToEdge
                                        y: WrapMode.ClampToEdge
                                    }
                                    comparisonFunction: Texture.CompareLessEqual
                                    comparisonMode: Texture.CompareNone
                                }
                            }
                        ]
                    }

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

            RenderPassFilter {
                matchAny: FilterKey { name: "pass"; value: "picking" }
                ClearBuffers {
                    clearColor: "#000"
                    buffers: ClearBuffers.ColorDepthBuffer
                    RenderCapture {
                        id: renderCapture
                        CameraSelector {
                            camera: root.camera
                        }
                    }
                }
            }

            RenderPassFilter {
                matchAny : FilterKey { name : "pass"; value : "final" }
                ClearBuffers {
                    clearColor: "#000"
                    buffers: ClearBuffers.ColorDepthBuffer
                    CameraSelector {
                        camera: root.camera
                    }
                }
            }
        }
    }
}
