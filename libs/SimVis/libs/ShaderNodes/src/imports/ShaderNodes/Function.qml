import ShaderNodes 1.0

ShaderNode {
    property var input: surface.geometryShader.position

    exportedTypeName: "Function"

    name: "function"
    type: "float"
    result: "sin(2.0 * $position.x - $position.y*$position.z) + cos(2.0 * $position.y) + sin(2.0 * $position.z)*cos(2.0*$position.y) + cos(cp_time) + sin(cos(cp_time)*tan(cp_time));"
}
