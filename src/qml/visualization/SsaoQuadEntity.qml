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
    
    property var deferredFrameGraph
    property var spheres
    property alias ambientOcclusion: ambientOcclusion
    
    Material {
        id: ssaoMaterial
        parameters : [
            Parameter { name: "normalTexture"; value : deferredFrameGraph.normalTexture },
            Parameter { name: "positionTexture"; value : deferredFrameGraph.positionTexture },
            Parameter { name: "colorTexture"; value : deferredFrameGraph.colorTexture },
            Parameter { name: "depthTexture"; value : deferredFrameGraph.depthTexture },
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
                        id: builderRenderPass
                        filterKeys : FilterKey { name : "pass"; value : "ssao" }
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
                            fragmentShaderCode: ssaoShaderBuilder.finalShader
                        }
                    }
                }
            ]
        }
        
        ShaderBuilder {
            id: ssaoShaderBuilder
            
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
            property ShaderNode color: ShaderNode {
                type: "vec4"
                name: "color"
                result: "color"
            }
            
            renderPass: builderRenderPass
            source: "
#version 330
"
            +(Qt.platform.os=="osxxx" ? "#define MACOSX" : "")+"
uniform highp sampler2D normalTexture;
uniform highp sampler2D positionTexture;
uniform highp sampler2D colorTexture;
uniform highp sampler2D depthTexture;
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
    // vec3 position = eyePosition + posMin + texture(positionTexture, texCoord).xyz * deltaMaxMin;
    vec3 position = texture(positionTexture, texCoord).xyz;
    vec4 color = texture(colorTexture, texCoord);
#endif

    if(depth > 1.0 - 1e-5) {
        discard;
    }

#pragma shadernodes body
}

"
            
            outputs: [
                ShaderOutput {
                    name: "fragColor"
                    type: "vec4"
                    value: Mix {
                        mix: 0.3
                        value1: AmbientOcclusion {
                            id: ambientOcclusion
                            cutoff: 4.0
                            samples: 64
                            radius: 10
                            depthTexture: deferredFrameGraph.depthTexture
                            mode: "hemisphere"
                        }
                        value2: AmbientOcclusion {
                            id: ambientOcclusion2
                            samples: ambientOcclusion.samples
                            radius: ambientOcclusion.radius / 2.0
                            depthTexture: ambientOcclusion.depthTexture
                            mode: ambientOcclusion.mode
                        }
                    }
                }
            ]
        }
        
    }
    
    components: [
        //                ssaoLayer,
        ssaoMaterial,
        quadMesh,
        quadTransform
    ]
}
