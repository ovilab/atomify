import SimVis 1.0
import ShaderNodes 1.0
import ShaderNodes 1.0 as Nodes

import Qt3D.Core 2.0
import Qt3D.Render 2.0

import QtQuick 2.0 as QQ2

Entity {
    id: spheresRoot
    property var layer
    property var variable: 0.0
    property string vertexShaderSourceFile: "qrc:/SimVis/render/shaders/gl3/spheres.vert"
    property string fragmentShaderSourceFile: "qrc:/SimVis/render/shaders/gl3/spheres.frag"
    //    property string vertexShaderSourceFile: "qrc:/SimVis/render/shaders/es2/spheres.vert"
    //    property string fragmentShaderSourceFile: "qrc:/SimVis/render/shaders/es2/spheres.frag"
    property alias fragmentColor: _fragmentColor.value
    property alias fragmentBuilder: _fragmentBuilder
    property alias normal: _fragmentBuilder.normal
    property alias position: _fragmentBuilder.position
    property alias posMin: posMin.value
    property alias posMax: posMax.value

    property SphereData sphereData
    property Camera camera

    onSphereDataChanged: {
        sphereData.parent = spheresRoot
    }

    Material {
        id: material
        parameters: [
            Parameter { id:posMin; name: "posMin"; value: 0.0 },
            Parameter { id:posMax; name: "posMax"; value: 200.0 }
        ]
        effect: Effect {
            techniques: [
                Technique {
                    graphicsApiFilter {
                        api: GraphicsApiFilter.OpenGL
                        profile: GraphicsApiFilter.CoreProfile
                        majorVersion: 3
                        minorVersion: 2
                    }
                    filterKeys: FilterKey { name: "renderingStyle"; value: "forward" }
                    renderPasses: RenderPass {
                        id: builderRenderPass
                        shaderProgram: ShaderProgram {
                            vertexShaderCode: loadSource(vertexShaderSourceFile)
                            fragmentShaderCode: _fragmentBuilder.finalShader
                        }
                        ShaderBuilder {
                            id: _fragmentBuilder

                            renderPass: builderRenderPass

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
                            property ShaderNode normalDotCamera: ShaderNode {
                                type: "vec3"
                                name: "normalDotCamera"
                                result: "normalDotCamera"
                            }
                            property ShaderNode sphereId: ShaderNode {
                                type: "float"
                                name: "sphereId"
                                result: "sphereId"
                            }
                            sourceFile: fragmentShaderSourceFile
                            outputs: [
                                ShaderOutput {
                                    id: _fragmentColor
                                    type: "vec4"
                                    name: "fragColor"
                                    value: StandardMaterial {
                                        color: _fragmentBuilder.color
                                    }
                                }
                            ]
                        }
                    }
                },
                Technique {
                    graphicsApiFilter {
                        api: GraphicsApiFilter.OpenGL
                        profile: GraphicsApiFilter.CoreProfile
                        majorVersion: 3
                        minorVersion: 2
                    }
                    filterKeys: FilterKey { name: "renderingStyle"; value: "deferred" }
                    renderPasses: RenderPass {
                        filterKeys: FilterKey { name: "pass"; value: "geometry" }
                        shaderProgram: ShaderProgram {
                            vertexShaderCode: loadSource("qrc:/SimVis/render/shaders/gl3/spheres-deferred.vert")
                            fragmentShaderCode: loadSource("qrc:/SimVis/render/shaders/gl3/spheres-deferred.frag")
                        }
                    }
                }
            ]
        }
    }
    GeometryRenderer {
        id: spheresMeshInstanced
        primitiveType: GeometryRenderer.TriangleStrip
        enabled: instanceCount != 0
        instanceCount: sphereData.count

        geometry: SpheresPointGeometry {
            attributes: [
                Attribute {
                    name: "pos"
                    attributeType: Attribute.VertexAttribute
                    vertexBaseType: Attribute.Float
                    vertexSize: 3
                    byteOffset: 0
                    byteStride: (3 + 3 + 1) * 4
                    divisor: 1
                    buffer: sphereData ? sphereData.buffer : null
                },
                Attribute {
                    name: "col"
                    attributeType: Attribute.VertexAttribute
                    vertexBaseType: Attribute.Float
                    vertexSize: 3
                    byteOffset: 3*4
                    byteStride: (3 + 3 + 1) * 4
                    divisor: 1
                    buffer: sphereData ? sphereData.buffer : null
                },
                Attribute {
                    name: "scale"
                    attributeType: Attribute.VertexAttribute
                    vertexBaseType: Attribute.Float
                    vertexSize: 1
                    byteOffset: (3+3)*4
                    byteStride: (3 + 3 + 1) * 4
                    divisor: 1
                    buffer: sphereData ? sphereData.buffer : null
                }
            ]
        }

    }

    components: [
        spheresMeshInstanced,
        material,
        layer
    ]
}
