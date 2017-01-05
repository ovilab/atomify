import QtQuick 2.7
import QtQuick.Controls.Material 2.0
import QtQuick.Controls 1.5
import QtQuick.Window 2.0
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0
import QtQml.StateMachine 1.0 as DSM
import Atomify 1.0
import SimVis 1.0
import Qt.labs.settings 1.0
import "../visualization"
import "editor"
import "RightBar"
import "../plotting"
Item {
    id: root

    property AtomifySimulator simulator: visualizer.simulator
    property alias visualizer: visualizer
    property alias renderQuality: visualizer.renderQuality
    property alias mouseMover: visualizer.mouseMover
    property bool focusMode: false

    signal releaseCursor()
    signal changeMode()
    signal focusViewport()
    signal unfocusViewport()
    signal captureCursor()

    function toggleFocusMode() {
        if(focusMode) {
            rightbar.width = 300
            rightbar.visible = true
            editor.visible = true
            focusMode = false
            tabDisable.hideTabDisable.start()
        } else {
            rightbar.width = 0
            editor.visible = false
            rightbar.visible = false
            focusMode = true
            tabDisable.showTabDisable.start()
        }
    }

    Component.onCompleted: {
        // editorTab.lammpsEditor.runScript()
    }

    DropArea {
        anchors.fill: parent
        onDropped: {
            if(drop.hasUrls) {
                var numUrls = drop.urls.length
                for(var i=0; i<drop.urls.length; i++) {
                    var url = drop.urls[i]
                    editor.editorWindow.openTab(url)
                }
            }
        }
    }

    SplitView {
        anchors.fill: parent

        Settings {
            property alias editorWidth: editor.width
            property alias rightbardWidth: rightbar.width
        }

        EditorPane {
            id: editor

            Layout.fillHeight: true
            Layout.minimumWidth: 200
            width: 400
            simulator: root.simulator
            visualizer: root.visualizer
        }

        Item {
            id: visualizerItem
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.minimumWidth: 100

            AtomifyVisualizer {
                id: visualizer
                rootItem: visualizerItem
                anchors.fill: parent
                onFocusChanged: {
                    if(focus) {
                        focusViewport()
                    } else {
                        unfocusViewport()
                    }
                }
                focusMode: root.focusMode
                mode: flymodeState.active ? "flymode" : "trackball"
                captureCursor: capturedState.active
            }

            MouseArea {
                id: flymodeOverlay
                anchors.fill: visualizer
                hoverEnabled: true
                propagateComposedEvents: true
                cursorShape: visualizer.mode == "flymode" && visualizer.captureCursor ? Qt.BlankCursor : undefined

                property real centerPointX: visualizerItem.width / 2
                property real centerPointY: visualizerItem.height / 2
                property real correctionX: 0
                property real correctionY: 0
                property real previousX: 0
                property real previousY: 0
                property bool ignoreNext: true

                function reset() {
                    ignoreNext = true
                    centerMouse()
                }

                function centerMouse() {
                    if(!root.mouseMover) {
                        return
                    }

                    var midPoint = getGlobalPosition(Qt.point(centerPointX, centerPointY), visualizerItem)
                    mouseMover.move(midPoint.x, midPoint.y)
                }

                function getGlobalPosition(p, item) {
                    var globalX = p.x
                    var globalY = p.y
                    while(item.parent !== undefined && item.parent !== null) {
                        globalX = globalX + item.x
                        globalY = globalY + item.y
                        item = item.parent
                    }
                    return Qt.point(globalX, globalY)
                }

                onPositionChanged: {
                    if(visualizer.mode !== "flymode" || !visualizer.captureCursor) {
                        return
                    }

                    // TODO fix sudden jitter on fast movement

                    if(ignoreNext) {
                        previousX = mouse.x
                        previousY = mouse.y
                        ignoreNext = false
                        return
                    }

                    var deltaX = mouse.x - previousX
                    var deltaY = mouse.y - previousY

                    visualizer.flymodePanTilt(deltaX + correctionX, -(deltaY + correctionY))

                    previousX = mouse.x
                    previousY = mouse.y

                    if(mouse.x > visualizerItem.width * 0.8 ||
                            mouse.x < visualizerItem.width * 0.2 ||
                            mouse.y > visualizerItem.height * 0.8 ||
                            mouse.y < visualizerItem.height * 0.2) {
                        correctionX = mouse.x - centerPointX
                        correctionY = mouse.y - centerPointY
                        centerMouse()
                    } else {
                        correctionX = 0
                        correctionY = 0
                    }
                }

                onPressed: {
                    mouse.accepted = false
                    if(visualizer.mode !== "flymode") {
                        return
                    }
                    captureCursor()
                    previousX = mouse.x
                    previousY = mouse.y
                }
            }

            Rectangle {
                anchors {
                    fill: parent
                    margins: 8
                }
                border {
                    width: 2.0
                    color: {
                        if(focusedState.active) {
                            if(capturedState.active) {
                                return Material.color(Material.Green)
                            } else {
                                return Material.accent
                            }
                        } else {
                            return Qt.rgba(1.0, 1.0, 1.0, 0.2)
                        }
                    }
                }
                color: "transparent"
            }

            MessageOverlay {
                id: messageOverlay
                property bool shouldBeVisible: simulator.states.idle.active || simulator.states.finished.active || simulator.states.crashed.active || simulator.states.reset.active

                anchors.fill: parent
                visible: false

                errorMessage: simulator.error
                welcome: simulator.states.idle.active
                finished: simulator.states.finished.active
                crashed: simulator.states.crashed.active
                cancelling: simulator.states.reset.active

                onContinueClicked: simulator.continued()
                onNewTabClicked: editor.editorWindow.newTab()
                onExamplesClicked: rightbar.showExamples()
                onHideClicked: visible = false
                onShouldBeVisibleChanged: {
                    if(shouldBeVisible) {
                        visible = true
                    } else {
                        visible = false
                    }
                }
            }

            ControlBar {
                id: controlBar1
                simulator: root.simulator
                visualizer: root.visualizer
                hidden: root.focusMode
                anchors.horizontalCenter: parent.horizontalCenter
                y: parent.height - 100
                width: 320
                height: 64
            }

            Row {
                anchors {
                    right: parent.right
                    bottom: parent.bottom
                    margins: 24
                }
                spacing: anchors.margins / 2

                Image {
                    source: "qrc:/images/reset_camera.png"
                    width: 48
                    height: width
                    fillMode: Image.PreserveAspectFit

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            flymodeOverlay.reset()
                            visualizer.resetToSystemCenter()
                        }
                    }
                }

                Image {
                    source: "qrc:/images/switch_camera.png"
                    width: 48
                    height: width
                    fillMode: Image.PreserveAspectFit

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            changeMode()
                        }
                    }
                }
            }

            ColorLegend {
                width: 50
                height: 300
                hidden: !visualizer.propertyModifier.active
                min: visualizer.propertyModifier.min
                max: visualizer.propertyModifier.max

                anchors {
                    top: parent.top
                    right: parent.right
                    margins: 30
                }
            }
        }

        RightBar {
            id: rightbar
            Layout.fillHeight: true
            Layout.minimumWidth: 200
            width: 300
            system: simulator.system
            visualizer: root.visualizer
        }
    }

    Item {
        id: shortcuts
        Shortcut {
            sequence: "Return"
            onActivated: messageOverlay.visible = false
        }

        Shortcut {
            sequence: "Ctrl+P"
            context: Qt.ApplicationShortcut
            onActivated: simulator.togglePaused()
        }
        Shortcut {
            sequence: "Space"
            context: Qt.ApplicationShortcut
            onActivated: simulator.togglePaused()
        }
        Shortcut {
            sequence: "Tab"
            context: Qt.ApplicationShortcut
            onActivated: toggleFocusMode()
        }
        Shortcut {
            sequence: "Ctrl+I"
            context: Qt.ApplicationShortcut
            enabled: !visualizer.focusMode
            onActivated: editor.editorWindow.currentEditor.textArea.forceActiveFocus()
        }
        Shortcut {
            sequence: "+"
            context: Qt.ApplicationShortcut
            onActivated: {
                if(simulator.simulationSpeed < 100) {
                    simulator.setSimulationSpeed(simulator.simulationSpeed+1)
                }
            }
        }
        Shortcut {
            sequence: "-"
            context: Qt.ApplicationShortcut
            onActivated: {
                if(simulator.simulationSpeed > 1) {
                    simulator.setSimulationSpeed(simulator.simulationSpeed-1)
                }
            }
        }
        Shortcut {
            sequence: "C"
            onActivated: {
                changeMode()
            }
        }

        Shortcut {
            sequence: "O"
            onActivated: {
                visualizer.resetToSystemCenter()
            }
        }

        Shortcut {
            sequence: "Escape"
            onActivated: {
                focusViewport()
                releaseCursor()
                messageOverlay.visible = false
                visualizer.forceActiveFocus()
            }
        }
    }

    DisableMessage {
        id: tabDisable
        x: parent.width*0.5 - 0.5*width
        y: 10
    }

    DSM.StateMachine {
        id: stateMachine
        running: true
        childMode: DSM.State.ParallelStates

        DSM.State {
            id: cameraStateGroup
            initialState: trackballState
            DSM.State {
                id: trackballState
                DSM.SignalTransition {
                    targetState: flymodeState
                    signal: changeMode
                }
                onEntered: {
                    console.log("Trackball entered")
                }
            }
            DSM.State {
                id: flymodeState
                DSM.SignalTransition {
                    targetState: trackballState
                    signal: changeMode
                }
                onEntered: {
                    console.log("Flymode entered")
                    flymodeOverlay.reset()
                }
            }
        }
        DSM.State {
            id: focusStateGroup
            initialState: focusedState
            DSM.State {
                id: unfocusedState
                DSM.SignalTransition {
                    targetState: focusedState
                    signal: focusViewport
                }
                onEntered: {
                    console.log("UnFocus entered")
                }
            }
            DSM.State {
                id: focusedState
                DSM.SignalTransition {
                    targetState: unfocusedState
                    signal: unfocusViewport
                }
                onEntered: {
                    console.log("Focus entered")
                }
            }
        }
        DSM.State {
            id: captureStateGroup
            initialState: uncapturedState
            DSM.State {
                id: capturedState
                DSM.SignalTransition {
                    targetState: uncapturedState
                    signal: unfocusViewport
                }
                DSM.SignalTransition {
                    targetState: uncapturedState
                    signal: releaseCursor
                }
                DSM.SignalTransition {
                    targetState: uncapturedState
                    signal: trackballState.entered
                }
                onEntered: {
                    console.log("captured entered")
                }
            }
            DSM.State {
                id: uncapturedState
                DSM.SignalTransition {
                    targetState: capturedState
                    signal: flymodeState.entered
                }
                DSM.SignalTransition {
                    targetState: capturedState
                    signal: captureCursor
                }
                onEntered: {
                    console.log("unCaptured entered")
                }
            }
        }
    }
}
