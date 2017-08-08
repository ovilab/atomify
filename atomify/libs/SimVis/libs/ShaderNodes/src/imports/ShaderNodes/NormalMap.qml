import ShaderNodes 1.0

ShaderNode {
    property var strength: 1.0
    property var color: Qt.vector3d(0.5, 0.5, 0.5)
    property var normal: ShaderBuilderBinding {
        property: "normal"
        defaultValue: Qt.vector3d(0.0, 0.0, 1.0)
    }
    property var tangent: ShaderBuilderBinding {
        property: "tangent"
        defaultValue: Qt.vector3d(0.0, 1.0, 0.0)
    }
    property var binormal: ShaderBuilderBinding {
        property: "binormal"
        defaultValue: Qt.vector3d(1.0, 0.0, 0.0)
    }

    exportedTypeName: "NormalMap"

    name: "normalmap"
    type: "vec3"
    result: "normalMap($(normal, vec3), $(tangent, vec3), $(binormal, vec3),
                       $(color, vec3), modelNormalMatrix, $(strength, float))"

    // TODO make matrix available to shader instead

    header: "
vec3 normalMap(vec3 normal, vec3 tangent, vec3 binormal, vec3 color, mat3 modelNormalMatrix, float strength) {
    vec3 regular = vec3(0.0, 0.0, 1.0);
    vec3 normColor = 2.0 * color - vec3(1.0, 1.0, 1.0);
    vec3 mcolor = (1.0 - strength) * regular + strength * normColor;
    vec3 result = normalize(mcolor[0] * tangent + mcolor[1] * binormal + mcolor[2] * normal);
    result = normalize(modelNormalMatrix * result);
    return result;
}
"
}
