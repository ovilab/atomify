import ShaderNodes 1.0

ShaderNode {
    property var value1: 0.0
    property var value2: 0.0
    property var mix: 0.5

    exportedTypeName: "Multiply"

    name: "multiply"
    type: {
        var type1 = glslType(value1)
        var type2 = glslType(value2)
        if(type1 === type2) {
            return type1
        }
        if(type1 === "float") {
            return type2
        }
        if(type2 === "float") {
            return type1
        }
        return type1
    }

    result: {
        var type1 = glslType(value1)
        var type2 = glslType(value2)
        if(type1 === type2 || type1 === "float" || type2 === "float") {
            return "$value1 * $value2"
        }
        return "$(value1, " + type + ") * $(value2, " + type + ")"
    }
}
