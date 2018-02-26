import Qt3D.Core 2.0
import Qt3D.Input 2.0
import Qt3D.Extras 2.0

Entity {
    id: root
    property var material
    property var layer
    property alias translation: transform.translation
    property alias rotationX: transform.rotationX
    property alias rotationY: transform.rotationY
    property alias rotationZ: transform.rotationZ
    property alias scale3D: transform.scale3D


    Transform {
        id: transform
    }

    components: [
        transform
    ]

    Entity {
        components: [
            coneMesh,
            material,
            layer,
            coneTransform
        ]
        ConeMesh {
            id: coneMesh
            bottomRadius: 0.3
            topRadius: 0.02
            length: 1.0
        }
        Transform {
            id: coneTransform
            translation: Qt.vector3d(0, cylinderMesh.length, 0)
        }
    }

    Entity {
        components: [
            cylinderMesh,
            material,
            layer,
            cylinderTransform
        ]
        CylinderMesh {
            id: cylinderMesh
            radius: 0.5*coneMesh.bottomRadius
            length: 3.0
        }
        Transform {
            id: cylinderTransform
            translation: Qt.vector3d(0, 0.5*cylinderMesh.length, 0)
        }
    }
}
