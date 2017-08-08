import ShaderNodes 1.0

ShaderNode {
    property var red: 0.0
    property var green: 0.0
    property var blue: 0.0

    exportedTypeName: "CombineRgb"

    name: "combine_rgb"
    type: "vec3"
    result: "vec3($(red, float), $(green, float), $(blue, float))"
}
