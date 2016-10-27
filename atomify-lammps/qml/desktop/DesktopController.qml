import QtQuick 2.5
import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Input 2.0
import Qt3D.Logic 2.0
import QtQml 2.2
import Atomify 1.0

Entity {
    id: root
    signal pressed
    property MouseMover mouseMover
    property var rootItem
    property Camera camera
    property real flymodeRotationSpeed: 1
    property real linearSpeed: 40.0
    property real lookSpeed: 500.0
    property real zoomSpeed: 20.0
    property real zoomLimit: 2.0
    property real flymodeSpeed: 1.0
    property real trackballSpeed: 1.0
    property real dragSpeed: 100.0
    property vector3d viewCenter: Qt.vector3d(0,0,0)
    property alias dragging: rightMouseButtonAction.active
    property string mode: "trackball"
    onModeChanged: {
        if(mode==="flymode") {
            centerMouse()
        }

        mouseMover.showCursor = mode==="trackball"
        if(mode === "trackball") {
            viewCenter = root.camera.viewCenter
        }
    }

    property bool active: !(mode==="flymode") && (
                          shiftAction.active ||
                          leftMouseButtonAction.active ||
                          rightMouseButtonAction.active ||
                          middleMouseButtonAction.active ||
                          Math.abs(keyboardXAxis.value) > 0 ||
                          Math.abs(keyboardYAxis.value) > 0 ||
                          Math.abs(keyboardZAxis.value) > 0 ||
                          Math.abs(keyboardTiltAxis.value) > 0)

    function getGlobalPosition(p, item) {
        var globalX = p.x
        var globalY = p.y
        while(item.parent != undefined) {
            globalX = globalX + item.x
            globalY = globalY + item.y
            item = item.parent
        }
        return Qt.point(globalX, globalY)
    }

    Shortcut {
        //TODO: use keyboard input instead
        sequence: "Shift+F"
        context: Qt.ApplicationShortcut
        onActivated: {
            if(root.mode === "flymode") {
                console.log("Switching mode to trackball")
                root.mode = "trackball"
            } else {
                console.log("Switching mode to flymode")
                root.mode = "flymode"
            }
        }
    }

    QtObject {
        id: d
        property vector3d firstPersonUp: Qt.vector3d(0, 0, 1)
    }

    function zoomDistance(firstPoint, secondPoint) {
        var u = secondPoint.minus(firstPoint); u = u.times(u);
        return u.x + u.y + u.z;
    }

    function centerMouse() {
        var midPoint = getGlobalPosition(Qt.point(rootItem.width*0.5, rootItem.height*0.5), rootItem)
        root.mouseMover.move(midPoint.x, midPoint.y)
    }

    MouseHandler {
        id: mouseHandler
        sourceDevice: mouseSourceDevice
        property bool resettingMousePosition: false
        property int centerPointX: rootItem.width*0.5
        property int centerPointY: rootItem.height*0.5
        property var lastX: -1
        property var lastY: -1

        onWheel: {
            if(mode === "flymode") {
                return
            }
            var scale = 1 - wheel.angleDelta.y / 1000
            if(1.0 - scale > 0.1) {
                scale = 0.9
            } else if(scale - 1.0 > 0.1) {
                scale = 1.1
            }

            var newPos = camera.viewCenter.minus(camera.viewVector.times(scale))
            if(newPos.minus(camera.viewCenter).length() < 1) {
                newPos = newPos.normalized()
            } else if(newPos.minus(camera.viewCenter).length() > 1e6) {
                return;
            }

            camera.position = newPos //TODO: translate?
        }

        onPositionChanged: {
            if(mode==="trackball") return

            var deltaX = mouse.x - centerPointX
            var deltaY = mouse.y - centerPointY

            if(deltaX == 0 && deltaY == 0) {
                flymodeRotationSpeed = 1
            } else {
                flymodeRotationSpeed = flymodeRotationSpeed*1.1
                flymodeRotationSpeed = Math.min(2.0, flymodeRotationSpeed)
            }

            root.camera.pan(deltaX/20 * flymodeRotationSpeed, d.firstPersonUp)
            root.camera.tilt(-deltaY/20 * flymodeRotationSpeed)
            centerMouse()
        }
    }

    KeyboardDevice {
        id: keyboardSourceDevice
    }

    MouseDevice {
        id: mouseSourceDevice

        sensitivity: 0.001
    }

    components: [
        LogicalDevice {
            actions: [
                Action {
                    id: noMouseButtonAction
                    ActionInput {
                        sourceDevice: mouseSourceDevice
                        buttons: [MouseEvent.NoButton]
                    }
                },
                Action {
                    id: leftMouseButtonAction
                    ActionInput {
                        sourceDevice: mouseSourceDevice
                        buttons: [MouseEvent.LeftButton]
                    }
                },
                Action {
                    id: rightMouseButtonAction
                    ActionInput {
                        sourceDevice: mouseSourceDevice
                        buttons: [MouseEvent.RightButton]
                    }
                },
                Action {
                    id: middleMouseButtonAction
                    ActionInput {
                        sourceDevice: mouseSourceDevice
                        buttons: [MouseEvent.MiddleButton]
                    }
                },
                Action {
                    id: shiftAction
                    ActionInput {
                        sourceDevice: keyboardSourceDevice
                        buttons: [Qt.Key_Shift]
                    }
                },
                Action {
                    id: controlAction
                    ActionInput {
                        sourceDevice: keyboardSourceDevice
                        buttons: [Qt.Key_Control]
                    }
                },
                Action {
                    id: altAction
                    ActionInput {
                        sourceDevice: keyboardSourceDevice
                        buttons: [Qt.Key_Alt]
                    }
                }
            ] // actions

            axes: [
                // Mouse
                Axis {
                    id: mouseXAxis
                    AnalogAxisInput {
                        sourceDevice: mouseSourceDevice
                        axis: MouseDevice.X
                    }
                },
                Axis {
                    id: mouseYAxis
                    AnalogAxisInput {
                        sourceDevice: mouseSourceDevice
                        axis: MouseDevice.Y
                    }
                },
                // Keyboard
                Axis {
                    id: keyboardXAxis
                    ButtonAxisInput {
                        sourceDevice: keyboardSourceDevice
                        buttons: [Qt.Key_A, Qt.Key_Left]
                        scale: -1.0
                    }
                    ButtonAxisInput {
                        sourceDevice: keyboardSourceDevice
                        buttons: [Qt.Key_D, Qt.Key_Right]
                        scale: 1.0
                    }
                },
                Axis {
                    id: keyboardYAxis
                    ButtonAxisInput {
                        sourceDevice: keyboardSourceDevice
                        buttons: [Qt.Key_W, Qt.Key_Up]
                        scale: d.shiftPressed ? 0.0 : 1.0
                    }
                    ButtonAxisInput {
                        sourceDevice: keyboardSourceDevice
                        buttons: [Qt.Key_S, Qt.Key_Down]
                        scale: d.shiftPressed ? 0.0 : -1.0
                    }
                },
                Axis {
                    id: keyboardZAxis
                    ButtonAxisInput {
                        sourceDevice: keyboardSourceDevice
                        buttons: [Qt.Key_X]
                        scale: d.shiftPressed ? 0.0 : 1.0
                    }
                    ButtonAxisInput {
                        sourceDevice: keyboardSourceDevice
                        buttons: [Qt.Key_Z]
                        scale: d.shiftPressed ? 0.0 : -1.0
                    }
                },
                Axis {
                    id: keyboardTiltAxis
                    ButtonAxisInput {
                        sourceDevice: keyboardSourceDevice
                        buttons: [Qt.Key_R]
                        scale: d.shiftPressed ? 0.0 : 1.0
                    }
                    ButtonAxisInput {
                        sourceDevice: keyboardSourceDevice
                        buttons: [Qt.Key_F]
                        scale: d.shiftPressed ? 0.0 : -1.0
                    }
                },
                Axis {
                    id: rollAxis
                    ButtonAxisInput {
                        sourceDevice: keyboardSourceDevice
                        buttons: [Qt.Key_E]
                        scale: d.shiftPressed ? 0.0 : 1.0
                    }
                    ButtonAxisInput {
                        sourceDevice: keyboardSourceDevice
                        buttons: [Qt.Key_Q]
                        scale: d.shiftPressed ? 0.0 : -1.0
                    }
                }
            ] // axes
        },
        FrameAction {
            property real timeSinceLastAction: 0.0

            function tiltAboutViewCenterWithLimits(tiltChange) {
                root.camera.tiltAboutViewCenter(tiltChange)
                if(d.firstPersonUp.dotProduct(camera.upVector) < 0) {
                    root.camera.tiltAboutViewCenter(-tiltChange)
//                    d.firstPersonUp = d.firstPersonUp.times(-1.0) // TODO consider doing this instead?
                }
            }

            onTriggered: {
                if(!root.enabled) {
                    return
                }

                if(leftMouseButtonAction.active || rightMouseButtonAction.active) {
                    pressed()
                }

                var flymodeFinalSpeed = flymodeSpeed * (shiftAction.active ? 5.0 : 1.0)
                var trackballFinalSpeed = trackballSpeed * (shiftAction.active ? 5.0 : 1.0)
                if(root.mode === "flymode") {
                    root.camera.translate(Qt.vector3d(keyboardXAxis.value*flymodeFinalSpeed, 0.0, keyboardYAxis.value*flymodeFinalSpeed))
                    root.camera.translate(Qt.vector3d(0, flymodeFinalSpeed*keyboardTiltAxis.value, 0))
                } else {
                    root.camera.panAboutViewCenter(keyboardXAxis.value * trackballFinalSpeed, d.firstPersonUp);
                    tiltAboutViewCenterWithLimits(keyboardTiltAxis.value * trackballFinalSpeed)

//                    var translationLength = keyboardYAxis.value * flymodeFinalSpeed
//                    var vvl = camera.position.minus(camera.viewCenter).length()
//                    // camera.viewVector.length()
//                    console.log("keyboardYAxis.value: ", keyboardYAxis.value)
//                    console.log("flymodeFinalSpeed: ", flymodeFinalSpeed)
//                    console.log("vvl: ", vvl)
//                    console.log("vvl - translationLength: ", vvl - translationLength)
//                    if(vvl - translationLength < 1.0) {
//                        translationLength = camera.viewVector.length() - 1.0
//                    }
//                    console.log("TL: ", translationLength)

                    root.camera.translate(Qt.vector3d(0.0, 0.0, keyboardYAxis.value * flymodeFinalSpeed), Camera.DontTranslateViewCenter);

                    if(rightMouseButtonAction.active) {
                       camera.translate(Qt.vector3d(-mouseXAxis.value * dragSpeed, -mouseYAxis.value * dragSpeed, 0.0))
                    }
                }

                // TODO Implement roll, see below
//                if(Math.abs(rollAxis.value) > 0.0) {
//                    root.camera.roll(rollAxis.value*flymodeFinalSpeed)
//                    d.firstPersonUp = root.camera.upVector
//                }

                if(!leftMouseButtonAction.active && !middleMouseButtonAction.active) {
                    timeSinceLastAction += dt
                    return
                }

                // The time difference since the last frame is passed in as the
                // argument dt. It is a floating point value in units of seconds.
                if (leftMouseButtonAction.active) {
                    if(timeSinceLastAction > 0.1) {
                        timeSinceLastAction = 0
                        return
                    }
                    root.camera.panAboutViewCenter(-mouseXAxis.value * lookSpeed, d.firstPersonUp);
                    tiltAboutViewCenterWithLimits(-mouseYAxis.value * lookSpeed);
                } else if(middleMouseButtonAction.active) {
                    if(timeSinceLastAction > 0.1) {
                        timeSinceLastAction = 0
                        return
                    }
                    var fov = root.camera.fieldOfView
                    fov += mouseYAxis.value * zoomSpeed
                    fov = Math.max(10.0, Math.min(160.0, fov))
                    root.camera.fieldOfView = fov
                }

//                var tanAngle = Math.tan(root.camera.fieldOfView / 2.0)
//                var distance = (root.camera.viewCenter.minus(root.camera.position)).length()
//                console.log("distance", distance, "sinAngle", tanAngle)
//                root.camera.right = distance * tanAngle
//                root.camera.left = -root.camera.right
//                root.camera.top = root.camera.right / root.camera.aspectRatio
//                root.camera.bottom = -root.camera.top
            }
        }
    ] // components
}
