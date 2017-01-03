import ShaderNodes 1.0
import ShaderNodes 1.0 as Nodes

import Qt3D.Core 2.0
import Qt3D.Render 2.0

import QtQuick 2.0 as QQ2
import QtQuick.Scene3D 2.0

ShaderBuilder {
    id: vertexShaderBuilder

    property alias positionOut: _position.value
    
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
