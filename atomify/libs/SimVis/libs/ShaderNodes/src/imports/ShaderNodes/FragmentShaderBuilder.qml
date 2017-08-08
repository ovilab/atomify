import ShaderNodes 1.0
import ShaderNodes 1.0 as Nodes

import Qt3D.Core 2.0
import Qt3D.Render 2.0

import QtQuick 2.0 as QQ2
import QtQuick.Scene3D 2.0

ShaderBuilder {
    id: fragmentShaderBuilder
    
    // inputs
    property alias position: position
    property alias normal: normal
    property alias tangent: tangent
    property alias binormal: binormal
    property alias textureCoordinate: textureCoordinate

    property alias fragmentColor: _fragmentColor.value
    
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
