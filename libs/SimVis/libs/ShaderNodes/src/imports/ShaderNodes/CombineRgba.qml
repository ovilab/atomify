import ShaderNodes 1.0

ShaderNode {
    property var red: 0.0
    property var green: 0.0
    property var blue: 0.0
    property var alpha: 0.0

    exportedTypeName: "CombineRgba"

    name: "combine_rgba"
    type: "vec4"
    result: "vec4($(red, float), $(green, float), $(blue, float), $(alpha, float))"
}
