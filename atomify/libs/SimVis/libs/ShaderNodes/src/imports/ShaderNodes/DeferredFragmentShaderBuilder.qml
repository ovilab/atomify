import ShaderNodes 1.0
import ShaderNodes 1.0 as Nodes

import Qt3D.Core 2.0
import Qt3D.Render 2.0

import QtQuick 2.0 as QQ2
import QtQuick.Scene3D 2.0

ShaderBuilder {
    id: deferredFragmentShaderBuilder

    property alias deferredColor: deferredColor.value
    
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
