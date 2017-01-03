import ShaderNodes 1.0
import ShaderNodes 1.0 as Nodes

import Qt3D.Core 2.0
import Qt3D.Render 2.0

import QtQuick 2.0 as QQ2
import QtQuick.Scene3D 2.0

ShaderBuilder {
    id: shaderBuilder

    property string version: "es2"
    
    shaderType: ShaderBuilder.Fragment
    
    // inputs
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
    property ShaderNode tangent: ShaderNode {
        type: "vec3"
        name: "tangent"
        result: "tangent"
    }
    property ShaderNode binormal: ShaderNode {
        type: "vec3"
        name: "binormal"
        result: "binormal"
    }
    property ShaderNode textureCoordinate: ShaderNode {
        type: "vec2"
        name: "texCoord"
        result: "texCoord"
    }
    
    sourceFile: "qrc:/ShaderNodes/shaders/" + version + "/shader_builder_material.frag"
    
    outputs: [
        ShaderOutput {
            id: _fragmentColor
            type: "vec4"
            name: "fragColor"
            value: StandardMaterial {
                position: shaderBuilder.position
                normal: shaderBuilder.normal
                lights: [
                    Nodes.Light {
                        position: Qt.vector3d(5.0, 5.0, -5.0)
                    }
                ]
            }
        }
    ]
}
