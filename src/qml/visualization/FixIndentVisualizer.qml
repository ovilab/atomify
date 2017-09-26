import Atomify 1.0

import Qt3D.Core 2.0
// import Qt3D.Render 2.0
// import Qt3D.Input 2.0
import Qt3D.Extras 2.0
// import Qt3D.Logic 2.0

import SimVis 1.0
import ShaderNodes 1.0

Entity {
    id: root
    property FixIndent fixIndent
    enabled: fixIndent ? fixIndent.hovered&&fixIndent.type==="sphere" : false

    components: [
        sphereMesh,
        whiteMaterial,
        viewCenterTransform,
        forwardFrameGraph.guideLayer
    ]
    SphereMesh {id: sphereMesh}
    ShaderBuilderMaterial {
        id: whiteMaterial
        fragmentColor: StandardMaterial {
            color: Qt.rgba(1.0, 1.0, 1.0, 0.9)
            lights: visualizer.lights
            ambientIntensity: 10.0
        }
    }
    Transform {
        id: viewCenterTransform
        translation: {
            if(fixIndent === undefined) return Qt.vector3d(0,0,0);
            if(fixIndent.type === "sphere") {
                return root.fixIndent.position
            }

        }
        scale: {
            if(fixIndent === undefined) return 1;
            if(fixIndent.type === "sphere") {
                return root.fixIndent.radius
            }
        }
    }
}
