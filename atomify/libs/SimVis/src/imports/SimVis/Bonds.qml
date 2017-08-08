import SimVis 1.0
import ShaderNodes 1.0
import ShaderNodes 1.0 as Nodes

import Qt3D.Core 2.0
import Qt3D.Render 2.0

import QtQuick 2.0 as QQ2

Entity {
    id: bondsRoot
    property var layer
    property var variable: 0.0
    property alias fragmentColor: _fragmentColor.value
    property alias fragmentBuilder: _fragmentBuilder
    property alias normal: _fragmentBuilder.normal
    property alias position: _fragmentBuilder.position
    property alias transform: transform
    property alias posMin: posMin.value
    property alias posMax: posMax.value
    property alias color: color.value

    property BondData bondData

    Transform {
        id: transform
    }

    Material {
        id: material
        parameters: [
            Parameter { id:posMin; name: "posMin"; value: 0.0 },
            Parameter { id:posMax; name: "posMax"; value: 200.0 },
            Parameter { id:color; name: "color"; value: Qt.vector3d(0.7, 0.7, 0.7) }
        ]
        effect: Effect {
            techniques: [
                Technique {
                    graphicsApiFilter {
                        api: GraphicsApiFilter.OpenGL
                        profile: GraphicsApiFilter.CoreProfile
                        minorVersion: 2
                        majorVersion: 3
                    }
                    filterKeys: FilterKey {
                        name: "renderingStyle"
                        value: "forward"
                    }
                    renderPasses: RenderPass {
                        id: builderRenderPass
                        shaderProgram: ShaderProgram {
                            vertexShaderCode: loadSource("qrc:/SimVis/render/shaders/gl3/bonds.vert")
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
                            property ShaderNode normalDotCamera: ShaderNode {
                                type: "vec3"
                                name: "normalDotCamera"
                                result: "normalDotCamera"
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

                            sourceFile: "qrc:/SimVis/render/shaders/gl3/bonds.frag"

                            outputs: [
                                ShaderOutput {
                                    id: _fragmentColor
                                    type: "vec4"
                                    name: "fragColor"
                                    value: StandardMaterial { }
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
                    filterKeys: FilterKey {
                        name: "renderingStyle"
                        value: "deferred"
                    }
                    renderPasses: RenderPass {
                        filterKeys: FilterKey { name: "pass"; value: "geometry" }
                        shaderProgram: ShaderProgram {
                            vertexShaderCode: loadSource("qrc:/SimVis/render/shaders/gl3/bonds.vert")
                            fragmentShaderCode: loadSource("qrc:/SimVis/render/shaders/gl3/bonds-deferred.frag")
                        }
                    }
                }
            ]
        }
    }
    GeometryRenderer {
        id: bondsMeshInstanced
        primitiveType: GeometryRenderer.TriangleStrip
        enabled: instanceCount != 0
        instanceCount: bondData ? bondData.count : 0

        geometry: PointGeometry {
            attributes: [
                Attribute {
                    name: "vertex1Position"
                    attributeType: Attribute.VertexAttribute
                    vertexBaseType: Attribute.Float
                    vertexSize: 3
                    byteOffset: 0
                    byteStride: (3 + 3 + 1 + 1 + 1 + 1) * 4
                    divisor: 1
                    buffer: bondData ? bondData.buffer : null
                },
                Attribute {
                    name: "vertex2Position"
                    attributeType: Attribute.VertexAttribute
                    vertexBaseType: Attribute.Float
                    vertexSize: 3
                    byteOffset: 3 * 4
                    byteStride: (3 + 3 + 1 + 1 + 1 + 1) * 4
                    divisor: 1
                    buffer: bondData ? bondData.buffer : null
                },
                Attribute {
                    name: "sphereRadius1"
                    attributeType: Attribute.VertexAttribute
                    vertexBaseType: Attribute.Float
                    vertexSize: 1
                    byteOffset: 6 * 4
                    byteStride: (3 + 3 + 1 + 1 + 1 + 1) * 4
                    divisor: 1
                    buffer: bondData ? bondData.buffer : null
                },
                Attribute {
                    name: "sphereRadius2"
                    attributeType: Attribute.VertexAttribute
                    vertexBaseType: Attribute.Float
                    vertexSize: 1
                    byteOffset: 7 * 4
                    byteStride: (3 + 3 + 1 + 1 + 1 + 1) * 4
                    divisor: 1
                    buffer: bondData.buffer
                },
                Attribute {
                    name: "radius1"
                    attributeType: Attribute.VertexAttribute
                    vertexBaseType: Attribute.Float
                    vertexSize: 1
                    byteOffset: 8 * 4
                    byteStride: (3 + 3 + 1 + 1 + 1 + 1) * 4
                    divisor: 1
                    buffer: bondData.buffer
                },
                Attribute {
                    name: "radius2"
                    attributeType: Attribute.VertexAttribute
                    vertexBaseType: Attribute.Float
                    vertexSize: 1
                    byteOffset: 9 * 4
                    byteStride: (3 + 3 + 1 + 1 + 1 + 1) * 4
                    divisor: 1
                    buffer: bondData ? bondData.buffer : null
                }
            ]
        }

    }

    components: [
        bondsMeshInstanced,
        material,
        transform,
        layer
    ]
}
