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
    property real width
    property real height
    
    Material {
        id: blurMaterial
        parameters : [
            Parameter { name: "ssaoTexture"; value : deferredFrameGraph.ssaoTexture },
            Parameter { name: "depthTexture"; value : deferredFrameGraph.depthTexture },
            Parameter { name: "blurSize"; value : 7 },
            Parameter { name: "winSize"; value : Qt.size(root.width, root.height) }
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
                        filterKeys : FilterKey { name : "pass"; value : "blur" }
                        shaderProgram : ShaderProgram {
                            vertexShaderCode: "
#version 330

uniform highp mat4 modelMatrix;

in highp vec4 vertexPosition;
in highp vec2 vertexTexCoord;

out highp vec2 texCoord;

void main()
{
    float scale = 0.4;
    texCoord = vertexTexCoord;
    texCoord.y = 1.0 - texCoord.y;
    gl_Position = modelMatrix * vertexPosition;
}
"
                            fragmentShaderCode: "
#version 330

uniform highp sampler2D ssaoTexture;
uniform highp sampler2D depthTexture;
uniform highp vec2 winSize;
uniform highp int blurSize;

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

highp float blurLinearizeDepth(highp float z) {
    highp float f="+root.farPlane+";
    highp float n = "+root.nearPlane+";

    return (2.0 * n) / (f + n - z * (f - n));
}

void main()
{
    highp vec2 scale = vec2(1.0 / winSize.x, 1.0 / winSize.y); // TODO make additional scale as parameter
    highp vec2 blurSizeHalf = vec2(float(blurSize) * 0.5);
    highp float fragDepth = blurLinearizeDepth(texture(depthTexture, texCoord).x);
    highp vec3 result = texture(ssaoTexture, texCoord).rgb;
    highp int sampleCount = 1;

    for(int i = 0; i < blurSize; i++) {
        for(int j = 0; j < blurSize; j++) {
            highp vec2 offset = scale * (vec2(i, j) - blurSizeHalf);
            highp vec3 value = texture(ssaoTexture, texCoord + offset).rgb;
            highp float sampleDepth = blurLinearizeDepth(texture(depthTexture, texCoord + offset).x);
            if(abs(fragDepth - sampleDepth) < 0.0001) { // TODO make into parameter
                result += value;
                sampleCount += 1;
            }
        }
    }
    fragColor = vec4(result / float(sampleCount), 1.0);
}
"
                        }
                        }
                        }
            ]
                        }
                        }
                            
                            components: [
                                blurMaterial,
                                quadMesh,
                                quadTransform
                            ]
                        }
                        
