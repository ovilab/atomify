import QtQuick 2.0
import SimVis 1.0
import ShaderNodes 1.0

AbstractMarchingCubes {
    id: functionSurfaceRoot
    property alias fragmentColor: shaderOutput.value
    property alias definition: geometryEval.value

    vertexShader: ShaderBuilder {
        sourceFile: "qrc:/org.compphys.SimVis/renderables/marchingcubes/marchingcubes.vsh"
        shaderType: ShaderBuilder.Vertex
    }

    geometryShader: ShaderBuilder {

        property ShaderNode position: ShaderNode {
            type: "vec3"
            name: "position"
            result: "position"
        }

        sourceFile: "qrc:/org.compphys.SimVis/renderables/marchingcubes/marchingcubes.gsh"
        shaderType: ShaderBuilder.Geometry

        outputs: [
            ShaderOutput {
                id: geometryEval
                type: "float"
                name: "shaderNodeResult"
                value: ShaderNode {
                    property var position: geometryShader.position
                    name: "position"
                    type: "float"
                    result: "sin(2.0 * $position.x - $position.y*$position.z) + cos(2.0 * $position.y) + sin(2.0 * $position.z)*cos(2.0*$position.y) + cos(cp_time) + sin(cos(cp_time)*tan(cp_time));"
                }
            }
        ]
    }

    fragmentShader: ShaderBuilder {
        id: _shader

        property ShaderNode position: ShaderNode {
            type: "vec3"
            name: "position"
            result: "position";
        }
        property ShaderNode normal: ShaderNode {
            type: "vec3"
            name: "normal"
            result: "normal";
        }

        sourceFile: "qrc:/org.compphys.SimVis/renderables/marchingcubes/marchingcubes.fsh"
        shaderType: ShaderBuilder.Fragment

        outputs: [
            ShaderOutput {
                id: shaderOutput
                type: "vec4"
                name: "cp_FragColor"
                value: StandardMaterial {
                    position: fragmentShader.position
                    color: "steelblue"
                    normal: fragmentShader.normal
                }
            }
        ]

        // TODO consider adding support for chaining shaders
        // TODO add functionality to choose input names or shader file based on GLSL version
    }
}

