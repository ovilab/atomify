import ShaderNodes 1.0

ShaderNode {
    property var value: 0.0

    exportedTypeName: "Sine"

    name: "sine"

    type: glslType(value)
    result: "sin($value)"
}
