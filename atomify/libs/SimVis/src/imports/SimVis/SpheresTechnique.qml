import Qt3D.Render 2.0
import SimVis 1.0
import ShaderNodes 1.0

Technique {
    id: rootTechnique
    property string vertexShaderSourceFile: "qrc:/SimVis/render/shaders/gl3/spheres.vert"
    property string fragmentShaderSourceFile: "qrc:/SimVis/render/shaders/gl3/spheres.frag"
    property alias fragmentColor: _fragmentColor.value
    property alias fragmentBuilderNormal: _fragmentBuilder.normal
    property alias fragmentBuilderPosition: _fragmentBuilder.position
    property alias fragmentBuilder: _fragmentBuilder
    renderPasses: RenderPass {
        bindings: [
            ParameterMapping {
                parameterName: "pos"
                shaderVariableName: "pos"
                bindingType: ParameterMapping.Attribute
            },
            ParameterMapping {
                parameterName: "vertexId"
                shaderVariableName: "vertexId"
                bindingType: ParameterMapping.Attribute
            }
        ]
        shaderProgram: ShaderProgram {
            vertexShaderCode: loadSource(rootTechnique.vertexShaderSourceFile)
            fragmentShaderCode: _fragmentBuilder.finalShader
            
            onFragmentShaderCodeChanged: {
                // console.log(fragmentShaderCode)
            }
        }
        ShaderBuilder {
            id: _fragmentBuilder
            
            material: material
            
            // TODO add readonly or some other way to show that these are only for others to read
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
            property ShaderNode textureCoordinate: ShaderNode {
                type: "vec2"
                name: "texCoord"
                result: "texCoord"
            }
            property ShaderNode color: ShaderNode {
                type: "vec3"
                name: "color"
                result: "color"
            }
            property ShaderNode sphereId: ShaderNode {
                type: "float"
                name: "sphereId"
                result: "sphereId"
            }
            
            sourceFile: rootTechnique.fragmentShaderSourceFile
            
            outputs: [
                ShaderOutput {
                    id: _fragmentColor
                    type: "vec4"
                    name: "fragColor"
                    value: StandardMaterial {
                        position: _fragmentBuilder.position
                        normal: _fragmentBuilder.normal
                        lights: [
                            Light {}
                        ]
                    }
                }
            ]
        }
    }
}
