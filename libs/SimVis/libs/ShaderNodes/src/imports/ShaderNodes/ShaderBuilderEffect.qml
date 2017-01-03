import ShaderNodes 1.0
import ShaderNodes 1.0 as Nodes

import Qt3D.Core 2.0
import Qt3D.Render 2.0

import QtQuick 2.0 as QQ2
import QtQuick.Scene3D 2.0

Effect {
    id: root

    property int sourceRgbArg: BlendEquationArguments.SourceAlpha
    property int destinationRgbArg: BlendEquationArguments.OneMinusSourceAlpha
    property int sourceAlphaArg: BlendEquationArguments.SourceAlpha
    property int destinationAlphaArg: BlendEquationArguments.OneMinusSourceAlpha

    property int blendFunctionArg: BlendEquation.Add

    property alias fragmentShaderBuilder: fragmentShaderBuilder
    property alias vertexShaderBuilder: vertexShaderBuilder
    property alias fragmentColor: fragmentShaderBuilder.fragmentColor
    property alias deferredColor: deferredFragmentShaderBuilder.deferredColor
    property alias vertexPosition: vertexShaderBuilder.positionOut

    techniques: [
        Technique {
            graphicsApiFilter {
                api: GraphicsApiFilter.OpenGL
                profile: GraphicsApiFilter.CoreProfile
                majorVersion: 3
                minorVersion: 3
            }
            filterKeys: FilterKey {
                name: "renderingStyle"
                value: "forward"
            }
            renderPasses: RenderPass {
                id: builderRenderPass
                shaderProgram: ShaderProgram {
                    vertexShaderCode: vertexShaderBuilder.finalShader
                    fragmentShaderCode: fragmentShaderBuilder.finalShader
                }               
                renderStates: [
                    BlendEquationArguments {
                        sourceRgb: sourceRgbArg
                        destinationRgb: destinationRgbArg
                        sourceAlpha: sourceAlphaArg
                        destinationAlpha: destinationAlphaArg
                    },
                    BlendEquation {
                        blendFunction: blendFunctionArg
                    }
                ]
            }
        },
        Technique {
            graphicsApiFilter {
                api: GraphicsApiFilter.OpenGL
                profile: GraphicsApiFilter.CoreProfile
                majorVersion: 3
                minorVersion: 3
            }
            filterKeys: FilterKey { name: "renderingStyle"; value: "deferred" }
            renderPasses: RenderPass {
                id: deferredGeometryRenderPass
                filterKeys: FilterKey { name: "pass"; value: "geometry" }
                shaderProgram : DeferredVertexShaderProgram {}
            }
        }
    ]
    VertexShaderBuilder {
        id: vertexShaderBuilder
    }
    FragmentShaderBuilder {
        id: fragmentShaderBuilder
    }
    DeferredFragmentShaderBuilder {
        id: deferredFragmentShaderBuilder
    }
}
