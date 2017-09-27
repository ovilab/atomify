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
    enabled: fixIndent ? fixIndent.hovered && (fixIndent.type==="sphere" || fixIndent.type==="cylinder") : false

    property string type: {
        if(!fixIndent) return "none"
        if(fixIndent.type==="sphere") return "sphere"
        if(fixIndent.type==="cylinder") return "cylinder"
        if(fixIndent.type==="plane") return "plane"
    }

    components: [
        sphereMesh,
        cylinderMesh,
        whiteMaterial,
        viewCenterTransform,
        forwardFrameGraph.guideLayer
    ]
    SphereMesh {
        id: sphereMesh
        enabled: root.type==="sphere"
        radius: fixIndent ? fixIndent.radius : 1.0
    }
    CylinderMesh {
        id: cylinderMesh
        enabled: root.type==="cylinder"
        radius: fixIndent ? fixIndent.radius : 1.0
        length: {
            if(!fixIndent) return 1.0
            if(fixIndent.dimension==0) return simulator.system.size.x
            else if(fixIndent.dimension==1) return simulator.system.size.y
            else return simulator.system.size.z
        }
    }
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
            if(!fixIndent) return Qt.vector3d(0,0,0);
            if(fixIndent.type === "sphere") {
                return root.fixIndent.position
            } else if(fixIndent.type === "cylinder") {
                var position = root.fixIndent.position
                // return position
                if(fixIndent.dimension===0) {
                    return position.plus(Qt.vector3d(simulator.system.size.x*0.5, 0, 0))
                } else if(fixIndent.dimension===1) {
                    return position.plus(Qt.vector3d(0, simulator.system.size.y*0.5, 0))
                } else {
                    return position.plus(Qt.vector3d(0, 0, simulator.system.size.z*0.5))
                }
            }

            return Qt.vector3d(0,0,0)
        }
        rotation: {
            if(!fixIndent) return Qt.quaternion(0,0,0,0)
            if(root.type==="cylinder") {
                if(fixIndent.dimension===0) {
                    return fromAxisAndAngle(Qt.vector3d(0, 0, 1), 90)
                } else if(fixIndent.dimension===1) {
                    return Qt.quaternion(0,0,0,0)
                } else if(fixIndent.dimension===2) {
                    return fromAxisAndAngle(Qt.vector3d(1, 0, 0), -90)
                }
            }

            return Qt.quaternion(0,0,0,0)
        }

//        scale: {
//            if(!fixIndent) return 1;
//            if(fixIndent.type === "sphere") {
//                return root.fixIndent.radius
//            }
//            return 1.0
//        }
    }
}
