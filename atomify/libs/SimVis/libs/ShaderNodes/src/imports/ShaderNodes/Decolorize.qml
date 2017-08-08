import ShaderNodes 1.0

ShaderNode {
    property var color: Qt.vector3d(1.0, 1.0, 1.0)

    exportedTypeName: "Decolorize"

    type: "float"
    name: "decolorize"
    result: "1.0 / (255.0 * 255.0) * ($(color, vec3).r * 255.0 * 255.0 + $(color, vec3).g * 255.0 + $(color, vec3).b)"
}
