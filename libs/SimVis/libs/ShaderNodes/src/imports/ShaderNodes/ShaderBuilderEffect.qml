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
    property alias fragmentColor: _fragmentColor.value
    property alias deferredColor: deferredColor.value
    property alias vertexPosition: _position.value

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
                shaderProgram : ShaderProgram {
                    vertexShaderCode:
                        "#version 330
in vec4 vertexPosition;
in vec3 vertexNormal;
out vec3 normal0;
out vec3 position0;
uniform mat4 mvp;
uniform mat4 modelMatrix;
uniform mat4 modelView;
uniform mat3 modelNormalMatrix;
uniform mat3 modelViewNormal;
uniform vec4 meshColor;
void main()
{
    position0 = (modelMatrix * vec4(vertexPosition.xyz, 1.0)).xyz ;
    normal0 = modelNormalMatrix * vertexNormal;
    gl_Position = mvp * vertexPosition;
}"
                    fragmentShaderCode: deferredFragmentShaderBuilder.finalShader
                }
            }
        }
    ]

    ShaderBuilder {
        id: vertexShaderBuilder

        // inputs
        property ShaderNode position: ShaderNode {
            type: "vec3"
            name: "position"
            result: "position"
        }
        property ShaderNode normal: ShaderNode {
            type: "vec3"
            name: "vertexNormal"
            result: "vertexNormal"
        }
        property ShaderNode textureCoordinate: ShaderNode {
            type: "vec2"
            name: "vertexTexCoord"
            result: "vertexTexCoord"
        }

        shaderType: ShaderBuilder.Vertex
        renderPass: builderRenderPass

        sourceFile: "qrc:/ShaderNodes/shaders/gl3/shader_builder_material.vert"

        outputs: [
            ShaderOutput {
                id: _position
                type: "vec3"
                name: "position"
                value: vertexShaderBuilder.position
            }
        ]
    }
    ShaderBuilder {
        id: fragmentShaderBuilder

        // inputs
        property alias position: position
        property alias normal: normal
        property alias tangent: tangent
        property alias binormal: binormal
        property alias textureCoordinate: textureCoordinate

        shaderType: ShaderBuilder.Fragment

        sourceFile: "qrc:/ShaderNodes/shaders/gl3/shader_builder_material.frag"
        renderPass: builderRenderPass

//        onFinalShaderChanged: console.log(finalShader)
//        QQ2.Component.onCompleted: console.log(finalShader)

        inputs: [
            ShaderNode {
                // TODO make a BuilderInputNode type
                id: position
                type: "vec3"
                name: "position"
                result: "position"
            },
            ShaderNode {
                id: normal
                type: "vec3"
                name: "normal"
                result: "normal"
            },
            ShaderNode {
                id: tangent
                type: "vec3"
                name: "tangent"
                result: "tangent"
            },
            ShaderNode {
                id: binormal
                type: "vec3"
                name: "binormal"
                result: "binormal"
            },
            ShaderNode {
                id: textureCoordinate
                type: "vec2"
                name: "texCoord"
                result: "texCoord"
            }
        ]
        outputs: [
            ShaderOutput {
                id: _fragmentColor
                type: "vec4"
                name: "fragColor"
                value: "purple"
            }
        ]
    }
    ShaderBuilder {
        id: deferredFragmentShaderBuilder

        shaderType: ShaderBuilder.Fragment

        source: "#version 330
in vec3 normal0;
in vec3 position0;

out vec4 normal;
out vec4 position;
out vec4 fragColor;

uniform vec3 eyePosition;
uniform vec3 viewVector;

#pragma shadernodes header

void main()
{
    float posMin = -100; // TODO should be set as uniforms
    float posMax = 100;
    float deltaMaxMin = posMax - posMin;
    position = vec4((position0-eyePosition-posMin) / deltaMaxMin, 1.0);
    normal = vec4((normalize(normal0) + 1.0) / 2.0, 1.0);

#pragma shadernodes body
}"
        renderPass: deferredGeometryRenderPass
        inputs: [
            ShaderNode {
                type: "vec3"
                name: "position"
                result: "position"
            },
            ShaderNode {
                type: "vec3"
                name: "normal"
                result: "normal"
            }
        ]

        outputs: [
            ShaderOutput {
                id: deferredColor
                type: "vec4"
                name: "fragColor"
                value: "purple"
            }
        ]
    }
}
