import ShaderNodes 1.0

ShaderNode {
    property var value: 0.0
    property var minimumValue: 0.0
    property var maximumValue: 1.0

    exportedTypeName: "Clamp"

    name: "clamp"
    type: glslType(value)
    result: "clamp($value, $(minimumValue, float), $(maximumValue, float))"
}
