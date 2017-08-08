import ShaderNodes 1.0

ShaderNode {
    property var strength: 0.1
    property var vector: Qt.vector3d(0.0, 0.0, 0.0)
    property var position: ShaderBuilderBinding {
        property: "position"
        defaultValue: Qt.vector3d(0.0, 0.0, 1.0)
    }

    exportedTypeName: "Displacement"

    name: "displacement"
    type: "vec3"
    result: "$(position, vec3) + $(strength, float) * (-1.0 + 2.0 * $(vector, vec3))"
}
