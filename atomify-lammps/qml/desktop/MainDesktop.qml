import QtQuick 2.7
import QtQuick.Controls.Material 2.0
import QtQuick.Controls 2.0
import QtQuick.Controls 1.5 as QQC1
import QtQuick.Window 2.0
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0
import StandardPaths 1.0
import QtQml.StateMachine 1.0 as DSM
import Atomify 1.0
import SimVis 1.0
import Qt.labs.settings 1.0
import "../visualization"
import "editor"
import "items"
import "RightBar"
import "../plotting"
import ".."

Rectangle {
    id: root

    property AtomifySimulator simulator: visualizer.simulator
    property alias visualizer: visualizer
    property alias renderQuality: visualizer.renderQuality
    property alias mouseMover: visualizer.mouseMover
    property alias editor: editor
    property string viewMode: "view"

    signal releaseCursor()
    signal changeMode()
    signal focusViewport()
    signal unfocusViewport()
    signal captureCursor()

    color: Material.background

    Timer {
        id: startsimtimer
        property string dataDir: StandardPaths.writableLocation(StandardPaths.AppDataLocation, ".")
        property string examplesDir: Qt.resolvedUrl(dataDir + "/examples")
        interval: 250
        onTriggered: {
            var initialExampleUrl = Qt.resolvedUrl(examplesDir + "/diffusion/diffusion/simple_diffusion.in")
//            visualizer.simulator.scriptFilePath = initialExampleUrl
//            visualizer.simulator.started()
            editor.editorWindow.openTab(initialExampleUrl)
            editor.editorWindow.runScript()
        }
    }

    function captureScreenshot() {
        visualizer.grabToImage(function(result) {
            fileDialogSave.callback = function() {
                // TODO: figure out why file:// doesn't work for saveToFile function
                result.saveToFile(fileDialogSave.fileUrl.toString().replace("file://",""));
            }
            fileDialogSave.visible = true
        })
    }

    Component.onCompleted: {
        startsimtimer.start()
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

    Rectangle {
        id: modeMenuContainer

        anchors {
            left: parent.left
            top: parent.top
            bottom: parent.bottom
        }
        color: Material.color(Material.Grey, Material.Shade900)
        onColorChanged: console.log("Color: ", color)
        width: 96

//        LinearGradient {
//            anchors.fill: parent
//            start: Qt.point(0, 0)
//            end: Qt.point(width, 10)
//            gradient: Gradient {
//                GradientStop {
//                    color: Material.color(Material.Grey, Material.Shade900)
//                    position: 0.0
//                }
//                GradientStop {
//                    color: Material.color(Material.Grey, Material.Shade900)
//                    position: 1.0
//                }
//            }
//        }

        Image {
            id: photonflowLogo
            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
                margins: 12
            }

            height: width
            mipmap: true
            smooth: true
            source: "qrc:/images/atomify_logo.png"
            fillMode: Image.PreserveAspectFit
        }

        Label {
            id: logoText
            anchors {
                horizontalCenter: parent.horizontalCenter
                top: photonflowLogo.bottom
                topMargin: 8
            }

            text: "Atomify\n2.0"
            font.pixelSize: 12
            horizontalAlignment: Text.AlignHCenter
        }

        Column {
            id: modeMenu

            property int currentIndex: 0

            onCurrentIndexChanged: {
                root.viewMode = modeModel.get(currentIndex).mode
                if(root.viewMode === "edit") messageOverlay.visible = false
            }

            Connections {
                target: root
                onViewModeChanged: {
                    for(var i = 0; i < modeModel.count; i++) {
                        if(modeModel.get(i).mode === root.viewMode) {
                            modeMenu.currentIndex = i
                        }
                    }
                }
            }

            anchors {
                verticalCenter: parent.verticalCenter
                left: parent.left
                right: parent.right
                margins: 16
            }
            spacing: 24

            Repeater {
                model: ListModel {
                    id: modeModel
                    ListElement {
                        image: "qrc:/images/ic_image_white_48dp.png"
                        name: "View"
                        mode: "view"
                    }
                    ListElement {
                        image: "qrc:/images/ic_timeline_white_48dp.png"
                        name: "Analyse"
                        mode: "analyse"
                    }
                    ListElement {
                        image: "qrc:/images/ic_mode_edit_white_48dp.png"
                        name: "Edit"
                        mode: "edit"
                    }
                    ListElement {
                        image: "qrc:/images/ic_view_quilt_white_48dp.png"
                        name: "Examples"
                        mode: "examples"
                    }
//                    ListElement {
//                        image: "qrc:/images/ic_help_white_48dp.png"
//                        name: "Help"
//                        mode: "help"
//                    }
                }

                ToggleButton {
                    anchors {
                        left: parent.left
                        right: parent.right
                    }
                    toggled: modeMenu.currentIndex === index
                    toolTipText: {
                        if(index===0) {
                            return "Focus on the visualization by hiding both the editor and analysis bar "+ctrl1.nativeText
                        } else if (index===1) {
                            return "Focus on the visualization and analysis by hiding the editor "+ctrl2.nativeText
                        } else if (index===2) {
                            return "Shows both the editor and the analysis "+ctrl3.nativeText
                        } else if (index===3) {
                            return "Explore the examples we've added to Atomify "+ctrl4.nativeText
                        }
//                        else if (index===4) {
//                            return "Shows the help pane "+ctrl5.nativeText
//                        }
                    }

                    source: image
                    text: name
                    onClicked: {
                        modeMenu.currentIndex = index
                    }
                }
            }
        }

        Column {
            anchors {
                bottom: parent.bottom
                left: parent.left
                right: parent.right
                margins: 16
            }
            spacing: 24

            ToggleButton {
                anchors {
                    left: parent.left
                    right: parent.right
                    margins: 8
                }
                source: {
                    if(simulator.states.paused.active || stopButton.toggled) {
                        return "qrc:/images/ic_play_arrow_white_48dp.png"
                    } else {
                        return "qrc:/images/ic_pause_white_48dp.png"
                    }
                }
                toggled: !stopButton.toggled
                text: {
                    if(stopButton.toggled) {
                        return "Simulate"
                    } else {
                        if(simulator.states.paused.active) {
                            return "Paused"
                        } else {
                            return "Running"
                        }
                    }
                }
                onClicked: {
                    if(simulator.states.idle.active) {
                        editor.editorWindow.runScript()
                    } else {
                        simulator.togglePaused()
                    }
                }
            }
            ToggleButton {
                id: stopButton
                anchors {
                    left: parent.left
                    right: parent.right
                    margins: 8
                }
                source: "qrc:/images/ic_stop_white_48dp.png"
                toggled: simulator.states.finished.active || simulator.states.idle.active
                text: toggled ? "Stopped" : "Stop"
                onClicked: {
                    simulator.reset()
                }
            }
            ToggleButton {
                anchors {
                    left: parent.left
                    right: parent.right
                    margins: 8
                }
                source: "qrc:/images/ic_refresh_white_48dp.png"
                text: "Restart"
                onClicked: {
                    editor.editorWindow.runScript()
                }
            }
        }
    }

    QQC1.SplitView {
        id: editView
        anchors {
            top: parent.top
            bottom: parent.bottom
            left: modeMenuContainer.right
            right: parent.right
        }
        visible: root.viewMode === "edit" || root.viewMode === "view" || root.viewMode == "analyse"
        handleDelegate: Item { width: 4 }

        Settings {
            property alias editorWidth: editor.width
            property alias rightbardWidth: rightbar.width
        }

        EditorPane {
            id: editor

            Layout.fillHeight: true
            Layout.minimumWidth: 200
            width: 600
            simulator: root.simulator
            visualizer: root.visualizer

            visible: root.viewMode === "edit"
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
                property bool shouldBeVisible: simulator.states.idle.active || simulator.states.finished.active || simulator.states.crashed.active || simulator.states.reset.active || simulator.welcomeSimulationRunning

                anchors.fill: parent
                visible: false

                errorMessage: simulator.error
                welcome: simulator.states.idle.active || simulator.welcomeSimulationRunning
                finished: simulator.states.finished.active
                crashed: simulator.states.crashed.active
                cancelling: simulator.states.reset.active

                onContinueClicked: simulator.continued()
                onEditClicked: modeMenu.currentIndex = 1
                onNewTabClicked: {
                    modeMenu.currentIndex = 1
                    editor.editorWindow.newTab()
                }

                onExamplesClicked: modeMenu.currentIndex = 2
                onHideClicked: visible = false
                onShouldBeVisibleChanged: {
                    if(shouldBeVisible) {
                        visible = true
                    } else {
                        visible = false
                    }
                }
            }

            Row {
                anchors {
                    right: parent.right
                    bottom: parent.bottom
                    margins: 24
                }
                spacing: anchors.margins / 2

                IconButton {
                    source: "qrc:/images/ic_center_focus_strong_white_36dp.png"
                    toolTipText: "Reset to origin [o]"

                    onClicked: {
                        if(flymodeState.active) {
                            flymodeOverlay.reset()
                        }
                        visualizer.resetToSystemCenter()
                    }
                }

                IconButton {
                    source: "qrc:/images/switch_camera.png"
                    toolTipText: "Switch camera [c]"

                    onClicked: {
                        changeMode()
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
            visible: root.viewMode === "edit" || root.viewMode == "analyse"
            width: 300
            system: root.simulator.system
            visualizer: root.visualizer
            simulator: root.simulator
        }
    }

    Examples {
        anchors {
            top: parent.top
            bottom: parent.bottom
            left: modeMenuContainer.right
            right: parent.right
        }
        visible: root.viewMode === "examples"
        onSimulationClicked: {
            editor.editorWindow.openTab(fileName)
            editor.editorWindow.runScript()
            root.viewMode = "view"
        }
    }

    Help {
        anchors {
            top: parent.top
            bottom: parent.bottom
            left: modeMenuContainer.right
            right: parent.right
        }
        visible: root.viewMode === "help"
    }

    EventMapper {
        mapping: {
            "desktop": root,
            "visualizer": visualizer,
            "simulator": visualizer.simulator
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
            sequence: "Ctrl+I"
            context: Qt.ApplicationShortcut
            onActivated: editor.editorWindow.currentEditor.textArea.forceActiveFocus()
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

        Shortcut {
            id: ctrl1
            sequence: "Ctrl+1"
            context: Qt.ApplicationShortcut
            onActivated: modeMenu.currentIndex = 0
        }

        Shortcut {
            sequence: "Ctrl+2"
            id: ctrl2
            context: Qt.ApplicationShortcut
            onActivated: modeMenu.currentIndex = 1
        }

        Shortcut {
            sequence: "Ctrl+3"
            id: ctrl3
            context: Qt.ApplicationShortcut
            onActivated: modeMenu.currentIndex = 2
        }

        Shortcut {
            sequence: "Ctrl+4"
            id: ctrl4
            context: Qt.ApplicationShortcut
            onActivated: modeMenu.currentIndex = 3
        }

//        Shortcut {
//            id: ctrl5
//            sequence: "Ctrl+5"
//            context: Qt.ApplicationShortcut
//            onActivated: modeMenu.currentIndex = 4
//        }

        Shortcut {
            sequence: "1"
            context: Qt.ApplicationShortcut
            onActivated: simulator.simulationSpeed = 1
        }

        Shortcut {
            sequence: "2"
            context: Qt.ApplicationShortcut
            onActivated: simulator.simulationSpeed = 5
        }

        Shortcut {
            sequence: "3"
            context: Qt.ApplicationShortcut
            onActivated: simulator.simulationSpeed = 30
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

    FileDialog {
        id: fileDialogSave
        selectExisting : false
        property var callback
        title: "Please choose a location to save"

        nameFilters: [ "Image files (*.jpg *.png)" ]

        onAccepted: {
            if(callback !== undefined) {
                console.log("Calling back")
                callback()
                callback = undefined
            }
        }
    }
}
