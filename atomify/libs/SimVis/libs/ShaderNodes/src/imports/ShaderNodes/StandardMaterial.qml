import QtQuick 2.5
import ShaderNodes 1.0

ShaderNode {
    property var color: "darkgrey"
    property var attenuationOffset: 0.0
    property var ambientColor: color
    property var diffuseColor: color
    property var specularColor: color
    property var ambientIntensity: 1.0
    property var diffuseIntensity: 1.0
    property var specularIntensity: 1.0
    property var hardness: 1.0

    property var normal: ShaderBuilderBinding {
        property: "normal"
        defaultValue: Qt.vector3d(0.0, 0.0, 1.0)
    }
    property var position: ShaderBuilderBinding {
        property: "position"
        defaultValue: Qt.vector3d(0.0, 0.0, 0.0)
    }

    property var lights: [_defaultLight]

    onLightsChanged: markDirty()

    property Light _defaultLight: Light {
        position: Qt.vector3d(46, 20, -10)
        attenuation: 0.0
        strength: 1.0
    }

    exportedTypeName: "StandardMaterial"

    arrayProperties: ["lights"]

    name: "diffuse"
    type: "vec4"
    source: {
        var output = ""
        output += "$this = vec4(0.0, 0.0, 0.0, 0.0);\n"
        output += "$this.a = $(diffuseColor, vec4).a;\n"

        for(var i in lights) {
            output += "$this.xyz += standardMaterialLight($lights[" + i + "], $(normal, vec3), $(position, vec3), eyePosition,\n"
            output += "             $(ambientColor, vec3), $(diffuseColor, vec3), $(specularColor, vec3),\n"
            output += "             0.01 * $(ambientIntensity, float), $(diffuseIntensity, float), 0.01 * $(specularIntensity, float),\n"
            output += "             150.0 * $(hardness, float), $(attenuationOffset, float));\n"
        }
        return output
    }

    headerFiles: [
        "qrc:/ShaderNodes/shaders/light.glsl",
        "qrc:/ShaderNodes/shaders/standard_material.glsl"
    ]
}
