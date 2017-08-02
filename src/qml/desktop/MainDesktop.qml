import QtQuick 2.7
import QtQuick.Controls.Material 2.0
import QtQuick.Controls 2.2
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
import "../events"
import "../plotting"
import ".."

Rectangle {
    id: root

    property AtomifySimulator simulator: visualizer.simulator
    property alias visualizer: visualizer
    property alias renderQuality: visualizer.renderQuality
    property alias backgroundColor: visualizer.backgroundColor
    property alias mouseMover: visualizer.mouseMover
    property alias editor: editor
    property string viewMode: "view"
    property string title: "Atomify"

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
                    EventCenter.postEvent("mode.edit")
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
        width: 72

        ColumnLayout {
            anchors {
                fill: parent
                topMargin: 16
                bottomMargin: 16
            }

            Item {
                Layout.fillHeight: true
                Layout.fillWidth: true
                Layout.minimumHeight: 16
                Layout.maximumHeight: modeMenuContainer.width
                Image {
                    anchors {
                        top: parent.top
                        bottom: logoLabel.top
                        left: parent.left
                        right: parent.right
                    }

                    mipmap: true
                    smooth: true
                    source: "qrc:/images/atomify_logo_inapp.png"
                    fillMode: Image.PreserveAspectFit
                    MouseArea {
                        anchors.fill: parent
                        hoverEnabled: true
                        onHoveredChanged: {
                            if(containsMouse) {
                                root.title = "Caffeinify"
                            } else {
                                root.title = "Atomify"
                            }
                        }
                    }
                }

                Label {
                    id: logoLabel
                    anchors {
                        left: parent.left
                        leftMargin: 8
                        rightMargin: 8
                        right: parent.right
                        bottom: parent.bottom
                    }

                    fontSizeMode: Text.Fit
                    height: 24

                    text: "Atomify<br>2.0.10"
                    horizontalAlignment: Text.AlignHCenter
                }
            }

            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true
            }

            Repeater {
                id: modeMenu

                property int currentIndex: 0

                onCurrentIndexChanged: {
                    root.viewMode = model[currentIndex].mode
                    if(root.viewMode === "edit") messageOverlay.hideClickedAtLeastOnce = true
                }

                Connections {
                    target: root
                    onViewModeChanged: {
                        for(var i in model) {
                            var mode = model[i].mode
                            if(mode === root.viewMode) {
                                modeMenu.currentIndex = i
                            }
                        }
                    }
                }

                model: [
                    {
                        image: "qrc:/images/ic_image_white_48dp.png",
                        name: "View",
                        mode: "view",
                        tooltip: "Show only visualization (" + EventCenter.nativeText("mode.view") + ")"
                    },
                    {
                        image: "qrc:/images/ic_timeline_white_48dp.png",
                        name: "Analyse",
                        mode: "analyse",
                        tooltip: "Show analysis (" + EventCenter.nativeText("mode.analyse") + ")"
                    },
                    {
                        image: "qrc:/images/ic_mode_edit_white_48dp.png",
                        name: "Edit",
                        mode: "edit",
                        tooltip: "Show both editor and analysis  (" + EventCenter.nativeText("mode.edit") + ")"
                    },
                    {
                        image: "qrc:/images/ic_view_quilt_white_48dp.png",
                        name: "Examples",
                        mode: "examples",
                        tooltip: "Explore examples  (" + EventCenter.nativeText("mode.examples") + ")"
                    }
                ]

                ToggleButton {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    Layout.minimumHeight: 32
                    Layout.maximumHeight: modeMenuContainer.width
                    toggled: modeMenu.currentIndex === index
                    toolTipText: modelData.tooltip

                    source: modelData.image
                    text: modelData.name
                    onClicked: {
                        modeMenu.currentIndex = index
                    }
                }
            }

            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true
            }

            ToggleButton {
                Layout.fillHeight: true
                Layout.fillWidth: true
                Layout.minimumHeight: 48
                Layout.maximumHeight: modeMenuContainer.width
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
                toolTipText: {
                    if(stopButton.toggled) {
                        return " Start simulating current script ("+EventCenter.nativeText("editorWindow.runScript")+")"
                    } else {
                        if(simulator.states.paused.active) {
                            return "Resume current simulation ("+EventCenter.nativeText("simulator.togglePause")+")"
                        } else {
                            return "Pause current simulation ("+EventCenter.nativeText("simulator.togglePause")+")"
                        }
                    }
                }

                onClicked: {
                    if(simulator.states.idle.active) {
                        editor.editorWindow.runScript()
                    } else {
                        simulator.togglePause()
                    }
                }
            }
            ToggleButton {
                id: stopButton
                Layout.fillHeight: true
                Layout.fillWidth: true
                Layout.minimumHeight: 48
                Layout.maximumHeight: modeMenuContainer.width
                source: "qrc:/images/ic_stop_white_48dp.png"
                toggled: simulator.states.finished.active || simulator.states.idle.active
                text: toggled ? "Stopped" : "Stop"
                onClicked: {
                    simulator.reset()
                }
                toolTipText: {
                    return "Stop current simulation"
                }
            }
            ToggleButton {
                Layout.fillHeight: true
                Layout.fillWidth: true
                Layout.minimumHeight: 48
                Layout.maximumHeight: modeMenuContainer.width
                source: "qrc:/images/ic_refresh_white_48dp.png"
                text: "Restart"
                onClicked: {
                    editor.editorWindow.runScript()
                }
                toolTipText: {
                    return "Restart current simulation ("+EventCenter.nativeText("editorWindow.runScript")+")"
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
                property bool shouldBeVisible: simulator.states.finished.active || simulator.states.crashed.active || simulator.states.reset.active || welcome || cantWrite
                onShouldBeVisibleChanged: {
                    if(shouldBeVisible) {
                        visible = true
                    } else {
                        visible = false
                    }
                }

                anchors.fill: parent
                visible: false

                errorMessage: simulator.error
                welcome: (simulator.states.idle.active || simulator.welcomeSimulationRunning) && !hideClickedAtLeastOnce
                finished: simulator.states.finished.active
                crashed: simulator.states.crashed.active
                cancelling: simulator.states.reset.active

                onContinueClicked: simulator.continued()
                onEditClicked: modeMenu.currentIndex = 2
                onNewTabClicked: {
                    modeMenu.currentIndex = 2
                    editor.editorWindow.newTab()
                }
                onOpenClicked: {
                    modeMenu.currentIndex = 2
                    editor.editorWindow.openTab()
                }

                onExamplesClicked: modeMenu.currentIndex = 3
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
                    toolTipText: "Reset to origin ("+EventCenter.nativeText("visualizer.resetToSystemCenter")+")"

                    onClicked: {
                        if(flymodeState.active) {
                            flymodeOverlay.reset()
                        }
                        visualizer.resetToSystemCenter()
                    }
                }

                IconButton {
                    source: "qrc:/images/switch_camera.png"
                    toolTipText: "Switch camera ("+EventCenter.nativeText("desktop.changeMode")+")"

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
            EventCenter.postEvent("editor.open", fileName)
            EventCenter.postEvent("editor.run")
            EventCenter.postEvent("mode.view")
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
            return {
                "desktop": root,
                "visualizer": visualizer
            }
        }
    }

    Item {
        id: shortcuts
        Shortcut {
            sequence: "Return"
            onActivated: messageOverlay.hideClicked()
        }

        EventCatcher {
            name: "simulator.togglePause"
            onTriggered: simulator.togglePause()
        }


        EventCatcher {
            name: "editor.cantwrite"
            onTriggered: {
                console.log("Mac app store: ", simulator.system.macAppStore)
                if(simulator.system.macAppStore) {
                    messageOverlay.cantWrite = true
                }
            }
        }

        EventCatcher {
            name: "editor.focus"
            onTriggered: editor.editorWindow.currentEditor.textArea.forceActiveFocus()
        }

        EventCatcher {
            name: "escape"
            onTriggered: {
                focusViewport()
                releaseCursor()
                messageOverlay.visible = false
                visualizer.forceActiveFocus()
            }
        }

        EventCatcher {
            name: "mode.view"
            onTriggered: modeMenu.currentIndex = 0
        }

        EventCatcher {
            name: "mode.analyse"
            onTriggered: modeMenu.currentIndex = 1
        }

        EventCatcher {
            name: "mode.edit"
            onTriggered: modeMenu.currentIndex = 2
        }

        EventCatcher {
            name: "mode.examples"
            onTriggered: modeMenu.currentIndex = 3
        }

        EventCatcher {
            name: "simulator.speed.normal"
            onTriggered: simulator.simulationSpeed = 1
        }

        EventCatcher {
            name: "simulator.speed.fast"
            onTriggered: simulator.simulationSpeed = 5
        }

        EventCatcher {
            name: "simulator.speed.faster"
            onTriggered: simulator.simulationSpeed = 30
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
            }
            DSM.State {
                id: flymodeState
                DSM.SignalTransition {
                    targetState: trackballState
                    signal: changeMode
                }
                onEntered: {
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
            }
            DSM.State {
                id: focusedState
                DSM.SignalTransition {
                    targetState: unfocusedState
                    signal: unfocusViewport
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
            }
        }
    }

    FileDialog {
        id: fileDialogSave
        selectExisting : false
        property var callback
        title: "Please choose a location to save"
        // nameFilters: [ "Image files (*.jpg *.png)" ]
        nameFilters: [ "Image files (*.png)" ]

        onAccepted: {
            if(callback !== undefined) {
                callback()
                callback = undefined
            }
        }
    }
}
