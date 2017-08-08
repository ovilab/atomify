import ShaderNodes 1.0
import Qt3D.Render 2.0

ShaderNode {
    id: root
    property url imageSource
    property var vector: ShaderBuilderBinding {
        property: "textureCoordinate"
    }
//    property var vector: Qt.vector2d(0, 0)
    property var offset: Qt.vector2d(0, 0)
    property var _texture: Texture2D {
        id: diffuseTexture
        minificationFilter: Texture.LinearMipMapLinear
        magnificationFilter: Texture.Linear
        wrapMode {
            x: WrapMode.ClampToEdge
            y: WrapMode.ClampToEdge
        }
        generateMipMaps: true
        maximumAnisotropy: 16.0
        TextureImage {
            id: textureImage
            // TODO: Remove this once Qt3D doesn't crash if file doesn't exist
            source: "qrc:/ShaderNodes/images/empty.png"
        }
    }

    exportedTypeName: "ImageTexture"

    name: "imagetexture"
    type: "vec4"
    result: "texture($_texture, $(vector, vec2) + $(offset, vec2))"

    onImageSourceChanged: {
        // TODO: Reuse this once Qt3D doesn't crash if file doesn't exist
        if(ShaderUtils.fileExists(imageSource)) {
            textureImage.source = imageSource
        }
    }
}
