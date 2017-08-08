import ShaderNodes 1.0

ShaderNode {
    property var vector: 0.0
    property var alpha: 0.0

    exportedTypeName: "CombineRgba"

    name: "combine_rgb_vector_alpha"
    type: "vec4"
    result: "vec4($(vector, vec3), $(alpha, float))"
}
