import Qt3D.Core 2.0
import Qt3D.Input 2.0
import Qt3D.Extras 2.9

Entity {
    property var material
    property var layer
    property alias translation: transform.translation
    property alias rotationX: transform.rotationX
    property alias rotationY: transform.rotationY
    property alias rotationZ: transform.rotationZ
    property alias scale3D: transform.scale3D
    property alias text: textMesh.text

    Transform {
        id: transform
    }

    components: [
        textMesh,
        material,
        transform,
        layer
    ]

    ExtrudedTextMesh {
        id: textMesh
        text: "MyText"
        depth: 0.1
        font.family: "Helvetica"
    }
}
