import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1
import Atomify 1.0

import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Input 2.0
import Qt3D.Extras 2.0

import QtQuick.Scene3D 2.0

import SimVis 1.0
import ShaderNodes 1.0

import "../desktop" // TODO should be separate controllers for desktop and mobile

Scene3D {
    id: root
    signal changedRenderQuality
    property alias propertyModifier: propertyModifier
    property bool guidesVisible
    property bool systemBoxVisible
    property alias rootItem: controller.rootItem
    property alias mouseMover: controller.mouseMover
    property bool focusMode
    property alias visualizer: visualizer
    property alias controller: controller
    property alias simulator: simulator
    property alias light1: light1
    property alias light2: light2
    property real scale: 0.23
    property alias nearPlane: mainCamera.nearPlane
    property alias farPlane: mainCamera.farPlane
    property bool addPeriodicCopies: false
    property alias ambientOcclusion: ssaoQuadEntity.ambientOcclusion
    property alias finalShaderBuilder: finalQuadEntity.shaderBuilder
    property alias sphereScale: colorModifier.scale
    property real bondRadius: 0.1
    property alias periodicImages: periodicImages
    property string renderMode: "forward"
    property string renderQuality: "medium"
    property bool mainCompleted: false
    property MessageDialog dialog: MessageDialog {
        text: "Render quality will be changed when the application is restarted."
    }
    hoverEnabled: controller.mode==="flymode"
    onFocusModeChanged: {
        if(focusMode) {
            controller.mode = "flymode"
        } else {
            controller.mode = "trackball"
        }
    }

    multisample: true
    onRenderQualityChanged: {
        if(mainCompleted) {
            dialog.open()
            return
        }

        if(renderQuality === "low") {
            renderMode = "forward"
            spheres.fragmentColor = spheres.fragmentBuilder.normalDotCamera
            bonds.fragmentColor = bonds.fragmentBuilder.normalDotCamera
        } else if(renderQuality === "medium") {
            renderMode = "forward"
            spheres.fragmentColor = spheresMediumQuality
            bonds.fragmentColor = bondsMediumQuality
        } else if(renderQuality === "high") {
            renderMode = "deferred"
            ambientOcclusion.samples = 32
        }
    }

    onBondRadiusChanged: {
        if(simulator != undefined) {
            simulator.system.atoms.bondRadius = bondRadius
        }
    }

    aspects: ["render", "input", "logic"]

    Entity {
        id: visualizer
        property Spheres spheres: spheres
        property MouseDevice mouseSourceDevice
        property KeyboardDevice keyboardSourceDevice
        property var lights: [
            light1,
            light2
        ]
        property Camera camera: Camera {
            id: mainCamera
            projectionType: CameraLens.PerspectiveProjection
            // projectionType: CameraLens.OrthographicProjection
            fieldOfView: 50
            aspectRatio: root.width / root.height
            nearPlane : root.renderMode === "forward" ? 1.0 : 3.0
            farPlane : root.renderMode === "forward" ? 10000.0 : 300.0
            position: Qt.vector3d(0.0, 50.0, 0.0) // do not change without taking upvector into account
            viewCenter: Qt.vector3d(0, 0, 0) // do not change without taking upvector into account
            upVector: Qt.vector3d(0.0, 0.0, 1.0)
            onPositionChanged: {
                if(simulator != undefined) {
                    simulator.system.cameraPosition = position
                }
            }
            Component.onCompleted: {
                mainCamera.panAboutViewCenter(40, Qt.vector3d(0, 0, 1))
                mainCamera.tiltAboutViewCenter(30)
            }
        }

        Light {
            id: light1
            position: visualizer.camera.position.plus(
                          (visualizer.camera.viewVector.normalized().plus(
                               visualizer.camera.upVector.normalized()).plus(
                               visualizer.camera.viewVector.crossProduct(visualizer.camera.upVector)).normalized()).times(20))
            strength: 0.5
            attenuation: 0.1
        }
        Light {
            id: light2
            position: visualizer.camera.position.minus(
                          (visualizer.camera.viewVector.normalized().plus(
                               visualizer.camera.upVector.normalized()).plus(
                               visualizer.camera.viewVector.crossProduct(visualizer.camera.upVector)).normalized()).times(10))
            strength: 0.5
            attenuation: 2.0
        }

        DesktopController {
            id: controller
            camera: visualizer.camera
            onPressed: root.focus = true
        }


        ForwardFrameGraph {
            id: forwardFrameGraph
            //            surface: deferredFrameGraph.surface
            camera: mainCamera
        }

        DeferredFrameGraph {
            id: deferredFrameGraph
            camera: mainCamera
            width: Math.max(10, root.width, root.height)
            height: width
            surface: forwardFrameGraph.surface
        }
        components: [
            RenderSettings {
                id: renderSettings
                activeFrameGraph: root.renderMode == "deferred" ? deferredFrameGraph : forwardFrameGraph
            },
            InputSettings {
                id: inputSettings
            }
        ]

        PlaneMesh {
            id: quadMesh
            width: 2.0
            height: width
            meshResolution: Qt.size(2, 2)
        }

        Transform { // We rotate the plane so that it faces us
            id: quadTransform
            rotation: fromAxisAndAngle(Qt.vector3d(1, 0, 0), 90)
        }

        SsaoQuadEntity {
            id: ssaoQuadEntity
            deferredFrameGraph: deferredFrameGraph
            spheres: spheres
        }

        BlurQuadEntity {
            id: blurQuadEntity
            deferredFrameGraph: deferredFrameGraph
            spheres: spheres
            farPlane: visualizer.camera.farPlane
            nearPlane: visualizer.camera.nearPlane
            width: root.width
            height: root.height
        }

        FinalQuadEntity {
            id: finalQuadEntity
            deferredFrameGraph: deferredFrameGraph
            spheres: spheres
            width: root.width
            height: root.height
        }

        AtomifySimulator {
            id: simulator
            simulationSpeed: 1
            system.atoms.modifiers: [
                colorModifier,
                propertyModifier,
                groupModifier,
                regionModifier,
                periodicImages
            ]
        }

        ColorModifier {
            id: colorModifier
            scale: 0.2
        }

        GroupModifier {
            id: groupModifier
        }

        RegionModifier {
            id: regionModifier
        }

        PropertyModifier {
            id: propertyModifier
        }

        PeriodicImages {
            id: periodicImages
            numberOfCopiesX: 1
            numberOfCopiesY: 1
            numberOfCopiesZ: 1
        }

        StandardMaterial {
            id: spheresMediumQuality
            color: spheres.fragmentBuilder.color
            attenuationOffset: 0 //root.simulator.distanceToNearestAtom // TODO: fix flimring
            lights: visualizer.lights
        }

        StandardMaterial {
            id: bondsMediumQuality
            color: "white"
            lights: visualizer.lights
        }

        Spheres {
            id: spheres
            camera: visualizer.camera
            sphereData: simulator.system.atoms.sphereData
            // TODO: Is posMin/posMax +-100 ok? We don't need system size anymore since all positions are relative to camera
            posMin: -100
            posMax:  100
            fragmentColor: spheresMediumQuality
        }

        Bonds {
            id: bonds
            color: "white"
            bondData: simulator.system.atoms.bondData
            posMin: spheres.posMin
            posMax: spheres.posMax
            fragmentColor: bondsMediumQuality
        }

        Entity {
            enabled: root.guidesVisible
            components: [
                CylinderMesh {},
                ShaderBuilderMaterial {
                    fragmentColor: "red"
                },
                Transform {
                    translation: visualizer.camera.viewCenter
                    scale3D: Qt.vector3d(0.51, 1000, 0.51)
                    rotationZ: 90
                }
            ]
        }

        Entity {
            enabled: root.guidesVisible
            components: [
                CylinderMesh {},
                ShaderBuilderMaterial {
                    fragmentColor: "green"
                },
                Transform {
                    translation: visualizer.camera.viewCenter
                    scale3D: Qt.vector3d(0.52, 1000, 0.52)
                }
            ]
        }

        Entity {
            enabled: root.guidesVisible
            components: [
                CylinderMesh {},
                ShaderBuilderMaterial {
                    fragmentColor: "blue"
                },
                Transform {
                    translation: visualizer.camera.viewCenter
                    scale3D: Qt.vector3d(0.53, 1000, 0.53)
                    rotationX: 90
                }
            ]
        }

        SystemBox {
            id: systemBox

            enabled: root.systemBoxVisible
            size: root.simulator.system.size
            lights: visualizer.lights
        }
    }
}
