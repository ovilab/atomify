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
    property Camera camera
    property real linearSpeed: 40.0
    property real lookSpeed: 500.0
    property real zoomLimit: 2.0
    property real zoomSpeed: 3.0
    property real translationSpeed: 1.6
    property real panSpeed: 3.6
    property real dragSpeed: 100.0
    property vector3d viewCenter: root.camera.viewCenter
    property alias dragging: rightMouseButtonAction.active
    property bool alternative: false

    property bool active: (shiftAction.active ||
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

    function tiltAboutViewCenterWithLimits(tiltChange) {
        root.camera.tiltAboutViewCenter(tiltChange)
        if(d.firstPersonUp.dotProduct(camera.upVector) < 0) {
            console.log(camera.upVector, camera.position, camera.viewCenter, camera.viewVector)
            root.camera.tiltAboutViewCenter(-tiltChange)
        }
    }

    function zoomDistance(firstPoint, secondPoint) {
        var u = secondPoint.minus(firstPoint); u = u.times(u);
        return u.x + u.y + u.z;
    }

    function zoom(value) {
        var scale = 1.0 + value / 100
        var newViewVector = camera.viewVector.times(scale)
        var delta = newViewVector.minus(camera.viewVector)
        camera.translateWorld(delta, Camera.DontTranslateViewCenter)
    }

    function centerMouse() {
        if(!root.mouseMover) {
            return
        }

        var midPoint = getGlobalPosition(Qt.point(rootItem.width*0.5, rootItem.height*0.5), rootItem)
        root.mouseMover.move(midPoint.x, midPoint.y)
    }

    QtObject {
        id: d
        property vector3d firstPersonUp: Qt.vector3d(0, 0, 1)
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
            if(!root.enabled) {
                return
            }
            if(wheel.modifiers & Qt.ControlModifier) {
                tiltAnimator.increase(wheel.angleDelta.y * 0.003)
                panAnimator.increase(-wheel.angleDelta.x * 0.003)
            } else {
                scrollZoomAnimator.increase(wheel.angleDelta.y * 0.006)
            }
        }
    }

    SmoothAnimator {
        id: tiltAnimator
        duration: 0.10
        onStepped: {
            tiltAboutViewCenterWithLimits(velocity * dt * 60)
        }
    }

    SmoothAnimator {
        id: zoomAnimator

        duration: 0.10

        onStepped: {
            console.log(velocity)
            zoom(dt * 60 * velocity)
        }
    }

    SmoothAnimator {
        id: scrollZoomAnimator

        duration: 0.16

        onStepped: {
            zoom(dt * 60 * velocity)
        }
    }

    SmoothAnimator {
        id: panAnimator

        duration: 0.10

        onStepped: {
            camera.panAboutViewCenter(velocity * dt * 60, d.firstPersonUp)
        }
    }

    SmoothAnimator {
        id: forwardAnimator
        duration: 0.10
        onStepped: {
            var forwardInPlane = camera.viewVector.times(1.0)
            forwardInPlane.z = 0
            forwardInPlane = forwardInPlane.normalized()
            root.camera.translateWorld(forwardInPlane.times(velocity * dt * 60), Camera.TranslateViewCenter);
        }
    }

    SmoothAnimator {
        id: rightAnimator
        duration: 0.10
        onStepped: {
            var rightInPlane = camera.viewVector.crossProduct(camera.upVector)
            rightInPlane.z = 0
            rightInPlane = rightInPlane.normalized()
            root.camera.translateWorld(rightInPlane.times(velocity * dt * 60), Camera.TranslateViewCenter);
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
                },
                Axis {
                    id: keyboardZoomAxis
                    ButtonAxisInput {
                        sourceDevice: keyboardSourceDevice
                        buttons: [Qt.Key_Z]
                        scale: d.shiftPressed ? 0.0 : 1.0
                    }
                    ButtonAxisInput {
                        sourceDevice: keyboardSourceDevice
                        buttons: [Qt.Key_X]
                        scale: d.shiftPressed ? 0.0 : -1.0
                    }
                }
            ] // axes
        },
        FrameAction {
            property real timeSinceLastAction: 0.0

            onTriggered: {
                if(!root.enabled) {
                    return
                }

                if(leftMouseButtonAction.active || rightMouseButtonAction.active) {
                    pressed()
                }

//                var shiftFactor = shiftAction.active ? 3.0 : 1.0
                var shiftFactor = 1.0

                tiltAnimator.set(keyboardTiltAxis.value * shiftFactor * zoomSpeed)

                if(alternative) {
                    panAnimator.set(rollAxis.value * shiftFactor * panSpeed)
                    forwardAnimator.set(keyboardYAxis.value * shiftFactor * translationSpeed)
                    rightAnimator.set(keyboardXAxis.value * shiftFactor * translationSpeed)
                    zoomAnimator.set(keyboardZoomAxis.value * shiftFactor * zoomSpeed)
                } else {
                    if(Math.abs(keyboardXAxis.value) > 0) {
                        panAnimator.set(keyboardXAxis.value * panSpeed * shiftFactor)
                    }
                    if(Math.abs(keyboardYAxis.value) > 0) {
                        zoomAnimator.set(keyboardYAxis.value * shiftFactor * zoomSpeed)
                    }
                }

                panAnimator.step(dt)
                zoomAnimator.step(dt)
                scrollZoomAnimator.step(dt)
                tiltAnimator.step(dt)
                forwardAnimator.step(dt)
                rightAnimator.step(dt)

                if(rightMouseButtonAction.active) {
                    camera.translate(Qt.vector3d(-mouseXAxis.value * dragSpeed, -mouseYAxis.value * dragSpeed, 0.0))
                }
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
            }
        }
    ] // components
}
