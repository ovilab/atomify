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

import "../desktop" // TODO should be separate controllers for desktop and mobile

Entity {
    id : root

    property alias shaderBuilder: finalShaderBuilder
    property var deferredFrameGraph
    property var spheres
    property real width
    property real height

    Material {
        id: finalMaterial
        parameters : [
            Parameter { name: "blurTexture"; value : deferredFrameGraph.blurTexture },
            Parameter { name: "ssaoTexture"; value : deferredFrameGraph.ssaoTexture },
            Parameter { name: "normalTexture"; value : deferredFrameGraph.normalTexture },
            Parameter { name: "positionTexture"; value : deferredFrameGraph.positionTexture },
            Parameter { name: "colorTexture"; value : deferredFrameGraph.colorTexture },
            Parameter { name: "depthTexture"; value : deferredFrameGraph.depthTexture },
            Parameter { name: "winSize"; value : Qt.size(root.width, root.height) },
            Parameter { name: "posMin"; value: spheres.posMin },
            Parameter { name: "posMax"; value: spheres.posMax }
        ]
        effect: Effect {
            techniques : [
                Technique {
                    filterKeys: FilterKey {
                        name: "renderingStyle"
                        value: "deferred"
                    }
                    graphicsApiFilter {
                        api: GraphicsApiFilter.OpenGL
                        profile: GraphicsApiFilter.CoreProfile
                        majorVersion: 3
                        minorVersion: 2
                    }
                    renderPasses : RenderPass {
                        id: finalRenderPass
                        filterKeys : FilterKey { name : "pass"; value : "final" }
                        shaderProgram : ShaderProgram {
                            vertexShaderCode: "
#version 330

uniform highp mat4 modelMatrix;

in highp vec4 vertexPosition;
in highp vec2 vertexTexCoord;

out highp vec2 texCoord;

void main()
{
    texCoord = vertexTexCoord;
    texCoord.y = 1.0 - texCoord.y;
    gl_Position = modelMatrix * vertexPosition;
}
"
                            
                            fragmentShaderCode: finalShaderBuilder.finalShader
                            // onFragmentShaderCodeChanged: console.log(fragmentShaderCode)
                        }
                    }
                }
            ]
        }
        
        ShaderBuilder {
            id: finalShaderBuilder
            function selectOutput(outputName) {
                if(outputName === "blurMultiply" || outputName === "Normal") {
                    output.value = blurMultiply
                }
                if(outputName === "ssaoMultiply") {
                    output.value = ssaoMultiply
                }
                if(outputName === "blur" || outputName === "Blurred SEM") {
                    output.value = blurNode
                }
                
                if(outputName === "ssao" || outputName === "SEM") {
                    output.value = ssaoNode
                }
                if(outputName === "position") {
                    output.value = position
                }
                if(outputName === "color") {
                    output.value = color
                }
                if(outputName === "normal") {
                    output.value = normal
                }
            }
            
            property ShaderNode position: ShaderNode {
                type: "vec3"
                name: "position"
                result: "position"
            }
            property ShaderNode normal: ShaderNode {
                type: "vec3"
                name: "normal"
                result: "normal"
            }
            property ShaderNode ssao: ShaderNode {
                type: "vec3"
                name: "ssao"
                result: "ssao"
            }
            property ShaderNode blur: ShaderNode {
                type: "vec3"
                name: "blur"
                result: "blur"
            }
            property ShaderNode color: ShaderNode {
                type: "vec4"
                name: "color"
                result: "color"
            }
            
            renderPass: finalRenderPass
            
            outputs: [
                ShaderOutput {
                    id: output
                    name: "fragColor"
                    type: "vec4"
                    value: blurMultiply
                    // value: finalShaderBuilder
                }
            ]
            
            Multiply {
                id: ssaoMultiply
                value1: ssaoNode
                value2: standardMaterial
            }
            Multiply {
                id: blurMultiply
                value1: blurNode
                value2: standardMaterial
            }
            ShaderNode {
                id: blurNode
                property var blur: finalShaderBuilder.blur
                name: "ambientOcclusion"
                type: "vec3"
                result: "$(blur, vec3)"
            }
            ShaderNode {
                id: ssaoNode
                property var ssao: finalShaderBuilder.ssao
                name: "ambientOcclusion"
                type: "vec3"
                result: "$(ssao, vec3)"
            }
            StandardMaterial {
                id: standardMaterial
                color: finalShaderBuilder.color
                attenuationOffset: 0 //root.simulator.distanceToNearestAtom // TODO: fix flimring
                
                lights: [
                    light1,
                    light2
                ]
                
                Light {
                    id: lighta1
                    position: light1.position
                    strength: light1.strength
                    attenuation: light1.attenuation
                }
                Light {
                    id: lighta2
                    position: light2.position
                    strength: light2.strength
                    attenuation: light2.attenuation
                }
            }
            
            source: "
#version 330
"
            +(Qt.platform.os=="osx" ? "#define MACOSX" : "")+"

uniform highp sampler2D blurTexture;
uniform highp sampler2D ssaoTexture;
uniform highp sampler2D normalTexture;
uniform highp sampler2D positionTexture;
uniform highp sampler2D colorTexture;
uniform highp sampler2D depthTexture;
uniform highp vec2 winSize;
uniform float posMin;
uniform float posMax;

uniform highp mat4 inverseProjectionMatrix;
uniform highp mat4 projectionMatrix;
uniform highp mat4 inverseViewMatrix;
uniform highp mat4 mvp;
uniform highp mat4 viewMatrix;
uniform highp mat4 tangentMatrix;
uniform highp mat4 modelView;
uniform highp vec3 eyePosition;

in highp vec2 texCoord;
out highp vec4 fragColor;

#pragma shadernodes header

highp vec3 positionFromDepth(highp float z, highp vec2 texCoord) {
    highp float x = texCoord.x * 2.0 - 1.0;
    highp float y = texCoord.y * 2.0 - 1.0;
    highp vec4 projectedPos = vec4(x, y, z, 1.0f / z);
    highp vec4 positionVS = inverseProjectionMatrix * projectedPos;
    positionVS /= positionVS.w;
    highp vec4 positionModel = inverseViewMatrix * positionVS;
    return positionModel.xyz;
}

void main()
{
        highp vec3 normal = normalize(-1.0 + 2.0 * texture(normalTexture, texCoord).xyz);
        highp float depth = texture(depthTexture, texCoord).x;
        float deltaMaxMin = posMax - posMin;
#ifdef MACOSX
    vec3 position = eyePosition + posMin + texture(colorTexture, texCoord).xyz * deltaMaxMin;
    vec4 color = texture(positionTexture, texCoord);
#else
    vec3 position = eyePosition + posMin + texture(positionTexture, texCoord).xyz * deltaMaxMin;
    vec4 color = texture(colorTexture, texCoord);
#endif
        highp vec3 ssao = texture(ssaoTexture, texCoord).rgb;
        highp vec3 blur = texture(blurTexture, texCoord).rgb;

    if(depth > 1.0 - 1e-5) {
        discard;
    }

#pragma shadernodes body
}

"
        }
    }
    
    components: [
        //                finalLayer,
        quadMesh,
        quadTransform,
        finalMaterial
    ]
}
