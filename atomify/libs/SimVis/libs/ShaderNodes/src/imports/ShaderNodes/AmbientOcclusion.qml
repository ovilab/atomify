import ShaderNodes 1.0

import Qt3D.Render 2.0

ShaderNode {
    property var samples: 16
    property var radius: 0.5
    property var cutoff: 2.0
    property var contrast: 0.0
    property var noiseScale: 1.0
    property var depthTexture
    property var randomVectorTexture: Texture2D {
        width : 128
        height : 2
        minificationFilter: Texture.Linear
        magnificationFilter: Texture.Linear
        wrapMode {
            x: WrapMode.Repeat
            y: WrapMode.Repeat
        }
        generateMipMaps: false
        TextureImage {
            source: "qrc:/ShaderNodes/images/ambient.png"
        }
    }
    property var noiseTexture: Texture2D {
        width : 256
        height : 256
        minificationFilter: Texture.Linear
        magnificationFilter: Texture.Linear
        wrapMode {
            x: WrapMode.Repeat
            y: WrapMode.Repeat
        }
        generateMipMaps: false
        TextureImage {
            source: "qrc:/ShaderNodes/images/noise.png"
        }
    }
    property var position: ShaderBuilderBinding {
        property: "position"
        defaultValue: Qt.vector3d(0.0, 0.0, 0.0)
    }
    property var normal: ShaderBuilderBinding {
        property: "normal"
        defaultValue: Qt.vector3d(0.0, 0.0, 0.0)
    }
    property var noiseTextureCoordinate: ShaderNode {
        name: "noiseTextureCoordinate"
        type: "vec2"
        result: "texCoordFromPosition(position, viewMatrix, projectionMatrix) + viewMatrix[0][0] + viewMatrix[0][1] + viewMatrix[0][2]"
    }

    property string mode: "hemisphere"
    property int modeNumber: mode === "hemisphere" ? 1 : 0

    exportedTypeName: "AmbientOcclusion"

    name: "ambient_occlusion"
    type: "float"
    result: "ambientOcclusion(
    $(depthTexture, sampler2D), $(noiseTexture, sampler2D), $(randomVectorTexture, sampler2D),
    $(position, vec3), $(normal, vec3), $(noiseTextureCoordinate, vec2),
    $(samples, int), $(radius, float), 10.0 * $(noiseScale, float), $(modeNumber, int),
    $(cutoff, float), $(contrast, float),
    viewMatrix, projectionMatrix)"

    headerFile: "qrc:/ShaderNodes/shaders/ambient-occlusion.glsl"
    // TODO make matrices explicit input
}
