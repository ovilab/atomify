import Qt3D.Core 2.0
import Qt3D.Input 2.0
import Qt3D.Extras 2.0

import ShaderNodes 1.0

Entity {
    id: root
    property var layer
    property var transform
    property var lights
    property alias translation: transform.translation
    property alias rotation: transform.rotation
    property alias scale3D: transform.scale3D

    Transform {
        id: transform
    }

    components: [
        transform
    ]

    ShaderBuilderMaterial {
        id: redMaterial
        fragmentColor: StandardMaterial {
            color: "red"
            lights: root.lights
            ambientIntensity: 10.0
        }
    }

    ShaderBuilderMaterial {
        id: greenMaterial
        fragmentColor: StandardMaterial {
            color: "green"
            lights: root.lights
            ambientIntensity: 10.0
        }
    }

    ShaderBuilderMaterial {
        id: blueMaterial
        fragmentColor: StandardMaterial {
            color: "blue"
            lights: root.lights
            ambientIntensity: 10.0
        }
    }

    Arrow {
        id: xArrow
        material: redMaterial
        layer: root.layer
        rotationZ: -90
    }

    Arrow {
        id: yArrow
        material: greenMaterial
        layer: root.layer
    }

    Arrow {
        id: zArrow
        material: blueMaterial
        layer: root.layer
        rotationX: 90
    }
}
