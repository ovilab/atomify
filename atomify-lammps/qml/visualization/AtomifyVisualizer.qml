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
    property var rootItem
    property MouseMover mouseMover
    property bool focusMode
    property alias visualizer: visualizer
    property alias controller: trackballController
    property alias simulator: simulator
    property alias light1: light1
    property alias light2: light2
    property real scale: 0.23
    property bool addPeriodicCopies: false
    property alias ambientOcclusion: ssaoQuadEntity.ambientOcclusion
    property alias finalShaderBuilder: finalQuadEntity.shaderBuilder
    property alias periodicImages: periodicImages
    property string renderMode: "forward"
    property string renderQuality: "medium"
    property bool mainCompleted: false
    property bool captureCursor: false
    property MessageDialog dialog: MessageDialog {
        text: "Render quality will be changed when the application is restarted."
    }
    property string mode: "trackball"

    hoverEnabled: root.mode === "flymode"
    multisample: true
    aspects: ["render", "input", "logic"]

    onFocusModeChanged: {
        if(focusMode) {
            root.mode = "flymode"
        } else {
            root.mode = "trackball"
        }
    }

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

    function changeMode() {
        if(mode === "flymode") {
            mode = "trackball"
        } else {
            mode = "flymode"
        }
    }

    function resetToSystemCenter() {
        // var right = camera.viewVector.normalized().cross(camera.upVector.normalized())
        var sizeY = Math.max(5.0, simulator.system.size.y)
        visualizer.camera.viewCenter = simulator.system.center
        visualizer.camera.position = simulator.system.center.plus(Qt.vector3d(0, 2*sizeY, 0))
        visualizer.camera.upVector = Qt.vector3d(0, 0, 1)
        // camera.translate(Qt.vector3d(0, 0, 10), Camera.DontTranslateViewCenter)
    }

    function flymodePanTilt(pan, tilt) {
        flymodeController.panTilt(pan, tilt)
    }

    Entity {
        id: visualizer
        property Spheres spheres: spheres
        property MouseDevice mouseSourceDevice
        property KeyboardDevice keyboardSourceDevice
        property var lights: [
            light1,
            light2
        ]
        function updateNearestPoint() {
            // Finds the projection of the camera position onto the system box
            // If camera is inside, nearestPoint is camera pos. If outside, one of the 6 faces on system box
            // It is used to have attenuation only work for relative coordinates inside the system so system
            //    is bright even when camera is far away.
            var x = camera.position.x
            var y = camera.position.y
            var z = camera.position.z


            var imageLowX = Math.floor( (periodicImages.numberOfCopiesX - 1)/2)
            var imageLowY = Math.floor( (periodicImages.numberOfCopiesY - 1)/2)
            var imageLowZ = Math.floor( (periodicImages.numberOfCopiesZ - 1)/2)

            var x0 = simulator.system.origin.x - simulator.system.size.x*imageLowX
            var y0 = simulator.system.origin.y - simulator.system.size.y*imageLowY
            var z0 = simulator.system.origin.z - simulator.system.size.z*imageLowZ

            var x1 = x0 + simulator.system.size.x*periodicImages.numberOfCopiesX
            var y1 = y0 + simulator.system.size.y*periodicImages.numberOfCopiesY
            var z1 = z0 + simulator.system.size.z*periodicImages.numberOfCopiesZ

            var xp = x
            var yp = y
            var zp = z
            if(x < x0) xp = x0
            if(x > x1) xp = x1
            if(y < y0) yp = y0
            if(y > y1) yp = y1
            if(z < z0) zp = z0
            if(z > z1) zp = z1
            nearestPoint = Qt.vector3d(xp, yp, zp)
        }

        property var nearestPoint
        property real distanceToNearestPoint: cameraPosition.minus(nearestPoint).length()
        property var cameraPosition: camera.position
        property Camera camera: root.mode === "flymode" ? flymodeCamera : trackballCamera

        onCameraPositionChanged: {
            if(simulator != undefined) {
                simulator.system.cameraPosition = cameraPosition
            }
            visualizer.updateNearestPoint()
        }


        Camera {
            id: trackballCamera
            Component.onCompleted: {
                panAboutViewCenter(40, Qt.vector3d(0, 0, 1))
                tiltAboutViewCenter(30)
            }
            projectionType: CameraLens.PerspectiveProjection
            fieldOfView: 50
            aspectRatio: root.width / root.height
            nearPlane : root.renderMode === "forward" ? 1.0 : 3.0
            farPlane : root.renderMode === "forward" ? 10000.0 : 300.0
            position: Qt.vector3d(0.0, 50.0, 0.0) // do not change without taking upvector into account
            viewCenter: Qt.vector3d(0, 0, 0) // do not change without taking upvector into account
            upVector: Qt.vector3d(0.0, 0.0, 1.0)
        }

        Camera {
            id: flymodeCamera

            function resetToTrackball() {
                position = trackballCamera.position
                viewCenter = trackballCamera.viewCenter
                upVector = trackballCamera.upVector
                fieldOfView = trackballCamera.fieldOfView
            }

            Component.onCompleted: {
                resetToTrackball()
            }

            projectionType: CameraLens.PerspectiveProjection
            nearPlane : root.renderMode === "forward" ? 1.0 : 3.0
            farPlane : root.renderMode === "forward" ? 10000.0 : 300.0
            aspectRatio: root.width / root.height
        }

        Light {
            id: light1
            position: visualizer.camera.position.plus(
                          (visualizer.camera.viewVector.normalized().plus(
                               visualizer.camera.upVector.normalized()).plus(
                               visualizer.camera.viewVector.crossProduct(visualizer.camera.upVector)).normalized()).times(20))
            strength: 0.5
            attenuation: 0.5
        }
        Light {
            id: light2
            position: visualizer.camera.position.minus(
                          (visualizer.camera.viewVector.normalized().plus(
                               visualizer.camera.upVector.normalized()).plus(
                               visualizer.camera.viewVector.crossProduct(visualizer.camera.upVector)).normalized()).times(10))
            strength: 0.5
            attenuation: 25.0
        }

        TrackballController {
            id: trackballController
            camera: trackballCamera
            onPressed: root.focus = true
            enabled: root.mode == "trackball"
        }

        FlymodeController {
            id: flymodeController
            camera: flymodeCamera
            onPressed: root.focus = true
            rootItem: root.rootItem
            mouseMover: root.mouseMover
            enabled: root.mode == "flymode"
        }

        Layer {
            id: atomLayer
        }

        Layer {
            id: guideLayer
        }

        Layer {
            id: outlineLayer
        }

        ForwardFrameGraph {
            id: forwardFrameGraph
            //            surface: deferredFrameGraph.surface
            camera: visualizer.camera
            atomLayer: atomLayer
            guideLayer: guideLayer
            outlineLayer: outlineLayer
        }

        DeferredFrameGraph {
            id: deferredFrameGraph
            camera: visualizer.camera
            width: Math.max(10, root.width, root.height)
            height: width
            surface: forwardFrameGraph.surface
            atomLayer: atomLayer
            guideLayer: guideLayer
            outlineLayer: outlineLayer
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
            system.onSizeChanged: { visualizer.updateNearestPoint() }
            system.onOriginChanged: { visualizer.updateNearestPoint() }

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
            onNumberOfCopiesXChanged: visualizer.updateNearestPoint()
            onNumberOfCopiesYChanged: visualizer.updateNearestPoint()
            onNumberOfCopiesZChanged: visualizer.updateNearestPoint()
        }

        StandardMaterial {
            id: spheresMediumQuality
            color: spheres.fragmentBuilder.color
            attenuationOffset: visualizer.distanceToNearestPoint
            lights: visualizer.lights
        }

        StandardMaterial {
            id: bondsMediumQuality
            color: "white"
            lights: visualizer.lights
        }

        Spheres {
            id: spheres
            layer: forwardFrameGraph.atomLayer
            camera: visualizer.camera
            sphereData: simulator.system.atoms.sphereData
            // TODO: Is posMin/posMax +-100 ok? We don't need system size anymore since all positions are relative to camera
            posMin: -100
            posMax:  100
            fragmentColor: spheresMediumQuality
        }

        Bonds {
            id: bonds
            layer: forwardFrameGraph.atomLayer
            color: "white"
            bondData: simulator.system.atoms.bondData
            posMin: spheres.posMin
            posMax: spheres.posMax
            fragmentColor: bondsMediumQuality
        }

        Guide {
            enabled: root.guidesVisible
            color: "red"
            scale3D: Qt.vector3d(0.51, 1000, 0.51)
            rotationZ: 90
            camera: visualizer.camera
            layer: forwardFrameGraph.guideLayer
        }

        Guide {
            enabled: root.guidesVisible
            color: "blue"
            scale3D: Qt.vector3d(0.53, 1000, 0.53)
            rotationX: 90
            camera: visualizer.camera
            layer: forwardFrameGraph.guideLayer
        }

        Guide {
            enabled: root.guidesVisible
            color: "green"
            scale3D: Qt.vector3d(0.52, 1000, 0.52)
            camera: visualizer.camera
            layer: forwardFrameGraph.guideLayer
        }

        Entity {
            enabled: trackballController.dragging
            components: [
                sphereMesh,
                whiteMaterial,
                viewCenterTransform,
                forwardFrameGraph.guideLayer
            ]
            SphereMesh {id: sphereMesh}
            ShaderBuilderMaterial {
                id: whiteMaterial
                fragmentColor: "white"
            }
            Transform {
                id: viewCenterTransform
                translation: visualizer.camera.viewCenter
            }
        }

        SystemBox {
            id: systemBox

            enabled: root.systemBoxVisible //|| controller.dragging
            origin: root.simulator.system.origin
            size: root.simulator.system.size
            corners: root.simulator.system.corners
            lights: visualizer.lights
            layer: forwardFrameGraph.outlineLayer
        }
    }
}
