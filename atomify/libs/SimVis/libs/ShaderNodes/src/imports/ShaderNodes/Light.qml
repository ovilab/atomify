import ShaderNodes 1.0

ShaderNode {
    // TODO split light into multiple functions available for different materials (should perhaps be on each material?)

    property var color: "white"
    property var strength: 1.0
    property var position: Qt.vector3d(0.0, 0.0, 0.0)

    property var attenuation: 1.0
    property var gamma: 1.0

    exportedTypeName: "Light"

    name: "light"
    type: "Light"
    source: "
$this.color = $(color, vec3);
$this.strength = $(strength, float);
$this.position = (vec4($(position, vec3), 1.0)).xyz;
$this.attenuation = $(attenuation, float);
$this.gamma = $(gamma, float);
"

    headerFiles: ["qrc:/ShaderNodes/shaders/light.glsl"]
}
