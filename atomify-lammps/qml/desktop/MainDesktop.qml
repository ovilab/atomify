import QtQuick 2.7
import QtQuick.Controls 1.5
import QtQuick.Window 2.0
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0
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

    Row {
        anchors.fill: parent

        SplitView {
            height: parent.height
            width: parent.width-rightbar.width
            Layout.alignment: Qt.AlignTop
            orientation: Qt.Horizontal

            EditorPane {
                id: editor
                Layout.fillHeight: true
                width: 500
                simulator: root.simulator
                visualizer: root.visualizer
                Component.onCompleted: {
                    simulator.crashed.connect(editor.reportError)
                }
            }

            Item {
                id: visualizerItem
                Layout.alignment: Qt.AlignLeft
                Layout.fillHeight: true
                Layout.minimumWidth: 1

                AtomifyVisualizer {
                    id: visualizer
                    rootItem: visualizerItem
                    anchors.fill: parent
                    focus: true
                    focusMode: root.focusMode
                }

                MouseArea {
                    id: flymodeOverlay
                    anchors.fill: visualizer
                    hoverEnabled: true
                    propagateComposedEvents: true

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
                        if(visualizer.mode !== "flymode") {
                            return
                        }

                        // TODO fix sudden jitter on fast movement

                        console.log("Previous", previousX, previousY)
                        console.log("Current", mouse.x, mouse.y)
                        console.log("Center", centerPointX, centerPointY)
                        console.log("Ignore", ignoreNext)

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

                    onPressed: mouse.accepted = false
                }

                MessageOverlay {
                    id: overlays
                    anchors {
                        left: parent.left
                        right: parent.right
                        leftMargin: parent.width*0.1
                        rightMargin: parent.width*0.1
                    }

                    visible: simulator.states.idle.active || simulator.states.finished.active
                    height: parent.height*0.2
                    y: parent.height*0.2

                    welcome: simulator.states.idle.active
                    finished: simulator.states.finished.active
                    onContinueClicked: simulator.continued()
                    onNewTabClicked: editor.editorWindow.newTab()
                    onExamplesClicked: rightbar.showExamples()
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
        }

        RightBar {
            id: rightbar

            width: 300
            height: parent.height
            system: simulator.system
            visualizer: root.visualizer
        }
    }

    Item {
        id: shortcuts
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
            sequence: "Alt+C"
            onActivated: {
                flymodeOverlay.reset()
                visualizer.changeMode()
            }
        }

        Shortcut {
            sequence: "Alt+R"
            onActivated: {
                visualizer.resetToSystemCenter()
            }
        }

        Shortcut {
            sequence: "Escape"
            onActivated: {
                visualizer.mode = "trackball"
                visualizer.focus = true
            }
        }
    }

    DisableMessage {
        id: tabDisable
        x: parent.width*0.5 - 0.5*width
        y: 10
    }
}
