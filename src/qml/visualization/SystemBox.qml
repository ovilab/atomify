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
    property real outlineAlpha: 0.6
    property real sideAlpha: 0.2
    property vector3d sizePlus: size.plus(Qt.vector3d(1, 1, 1))
    property var corners
    property var lights: [
        dummy
    ]

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
            //            surfaceTransform
        ]

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

        Material {
            id: surfaceMaterial
            parameters: [
                Parameter { name: "origin"; value: simulator.system.origin.times(simulator.system.atoms.globalScale) },
                Parameter { name: "transformationMatrix"; value: simulator.system.transformationMatrix; }
            ]
            effect: Effect {
                techniques: Technique {
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

                            vertexShaderCode: "
#version 330
uniform mat4 mvp;
uniform mat4 modelMatrix;
uniform mat4 modelView;
uniform mat3 modelNormalMatrix;
uniform mat3 modelViewNormal;
uniform vec4 meshColor;
uniform vec3 origin;
uniform mat4 transformationMatrix;

in vec4 vertexPosition;
in vec3 vertexNormal;
in vec2 vertexTexCoord;

out vec3 normal;
out vec3 position;
out vec2 texCoord;

void main()
{
    texCoord = vertexTexCoord;
    position = vertexPosition.xyz;
    position += vec3(0.5);
    mat3 m = mat3(transformationMatrix);
    m[0][0] += 1.0;
    m[1][1] += 1.0;
    m[2][2] += 1.0;
    position = m * position + origin - vec3(0.5);
    normal = modelNormalMatrix * vertexNormal;
    gl_Position = mvp * vec4(position, 1.0);
    position = (modelMatrix*vec4(position, 1.0)).xyz;
}"
                            fragmentShaderCode: fragmentShaderBuilder.finalShader
                            FragmentShaderBuilder {
                                id: fragmentShaderBuilder

                                ShaderNode {
                                    id: isOnEdge
                                    type: "float"
                                    result: "float(max(abs(texCoord.x - 0.5), abs(texCoord.y - 0.5)) > 0.49)"
                                }

                                fragmentColor: StandardMaterial {
                                    color: Add {
                                        value1: CombineRgbVectorAlpha {
                                            vector: Qt.rgba(0.5, 0.6, 0.8)
                                            alpha: Multiply {
                                                value1: root.outlineAlpha
                                                value2: isOnEdge
                                            }
                                        }
                                        value2: Qt.rgba(1.0, 1.0, 1.0, root.sideAlpha)
                                    }

                                    lights: root.lights
                                }
                            }
                        }
                        renderStates: [
                            BlendEquationArguments {
                                sourceRgb: BlendEquationArguments.SourceAlpha
                                destinationRgb: BlendEquationArguments.OneMinusSourceAlpha
                                sourceAlpha: BlendEquationArguments.SourceAlpha
                                destinationAlpha: BlendEquationArguments.OneMinusSourceAlpha
                            },
                            BlendEquation {
                                blendFunction: BlendEquation.Add
                            }
                        ]
                    }
                }
            }
        }
    }

}
