import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.0
import Qt.labs.settings 1.0
import QtQuick.Controls 1.5 as QQC1
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.2
import Atomify 1.0

import "../.."
import "../../events"

Item {
    id: root
    clip: true

    signal clearConsole()
    signal didRun()

    property alias dummyEditor: dummyEditor
    property AtomifySimulator simulator
    property CodeEditor currentEditor: (editorCount===0) ? null : stackLayout.itemAt(stackLayout.currentIndex)
    property CodeEditor activeEditor
    property alias editorCount: stackLayout.count
    property int currentLine: -1
    property int errorLine: -1
    property string openFiles: ""
    property real lastRunScript: 0
    property string lastOpenedFolder
    property bool applicationActive: Qt.application.state===Qt.ApplicationActive
    onApplicationActiveChanged: if(applicationActive && visible) { currentEditor.refresh() }
    onVisibleChanged: handleVisibleChanged()

    onCurrentEditorChanged: {
        focusCurrentEditor()
        if(currentEditor) currentEditor.refresh()
    }


    function handleVisibleChanged() {
        if(visible) {
            if(visualizer.simulator.scriptFilePath!=="") {
                openTab("file://"+visualizer.simulator.scriptFilePath)
            }
            focusCurrentEditor()
            currentEditor.refresh()
        }
    }

    onSimulatorChanged: {
        simulator.reset.connect(function() {
            editorWindow.clear()
        })
    }

    function runScript() {
        if(currentEditor.changedSinceLastSave) {
            saveChangesDialog.open()
            return;
        }

        var now = new Date().getTime()
        var dt = now - lastRunScript
        if(dt < 500) return; // Hack because I'm tired. The state machinery doesn't work perfectly. If Cmd+R is spammed it hangs without this hack fix

        if(simulator.states.reset.active) return;

        if(!simulator.states.idle.active) {
            simulator.reset()
            simulator.didReset.connect(runScript)
            return;
        }
        lastRunScript = now

        simulator.didReset.disconnect(runScript)
        editorWindow.activeEditor = editorWindow.currentEditor
        simulator.scriptFilePath = editorWindow.activeEditor.fileUrl
        simulator.started()
        didRun()
    }

    Component.onCompleted: {
        if(openFiles==="") {
            // First time app opens, this is an empty string which doesn't parse as array
            openFiles = "[]"
        }

        var files = JSON.parse(openFiles)

        var numOpenFiles = 0
        for(var i in files) {
            var filename = files[i]
            if(dummyEditor.fileExists(filename)) {
                openTab(filename)
                numOpenFiles += 1
            }
        }

        if(numOpenFiles === 0) {
            newTab()
        }
    }

    function updateOpenFiles() {
        var openFilesList = []

        for(var i=0; i<stackLayout.count; i++) {
            var editor = stackLayout.itemAt(i)
            openFilesList.push(editor.fileUrl.toString())
        }
        openFiles = JSON.stringify(openFilesList)
    }

    onCurrentLineChanged: {
        if(activeEditor != undefined) activeEditor.currentLine = currentLine
    }

    onErrorLineChanged: {
        if(activeEditor != undefined) activeEditor.errorLine = errorLine
    }

    function clear() {
        for(var i=0; i<stackLayout.count; i++) {
            stackLayout.itemAt(i).clear()
        }
    }

    function focusCurrentEditor() {
        if(currentEditor) {
            currentEditor.textArea.forceActiveFocus()
        }
    }

    function newTab() {
        var codeEditorComponent = Qt.createComponent("CodeEditor.qml")
        var newCodeEditor = codeEditorComponent.createObject(stackLayout);
        newCodeEditor.changedSinceLastSave = false
        stackLayout.currentIndex = fileListView.count - 1
        focusCurrentEditor()

        updateOpenFiles()
    }

    function showDoYouWantToSave(fileName) {
        messageDialog.text = "Do you want to save the changes you made to "+fileName+"?"
        messageDialog.visible = true
    }

    function closeTab() {
        if(currentEditor === null) {
            // TODO this should never happen, if it does, we should rather set currentEditor in a more permanent way
            console.log("Current editor is null, cannot close tab. Current index:",
                        stackLayout.currentIndex,
                        stackLayout.itemAt(stackLayout.currentIndex),
                        currentEditor)
            return;
        }

        if(currentEditor.changedSinceLastSave) {
            // Ask user to save the file before we close the tab
            messageDialog.cb = function() {
                // Callback is to close the tab
                var indexOfCurrentTab = stackLayout.currentIndex
                var editor = currentEditor
                currentEditor.parent = null
                editor.destroy()
                messageDialog.cb = null
            }

            showDoYouWantToSave(currentEditor.fileName)
            return
        } else {
            var indexOfCurrentTab = stackLayout.currentIndex
            var editor = currentEditor

            currentEditor.parent = null
            editor.destroy()
            messageDialog.cb = null
        }

        if(editorCount == 0) {
            newTab()
        }

        if(stackLayout.currentIndex >= editorCount) {
            stackLayout.currentIndex -= 1
        }

        updateOpenFiles()
        visualizer.simulator.scriptFilePath = ""
        focusCurrentEditor()
    }

    function openTab(filename, errorLine) {
        if(errorLine === undefined) {
            errorLine = -1
        }

        if(filename === undefined) {
            // If we just pressed Ctrl+O to open, show the dialog
            fileDialog.cb = function() {
                for(var i in fileDialog.fileUrls) {
                    var fileUrl = fileDialog.fileUrls[i]
                    openTab(fileUrl)
                }
            }
            fileDialog.folder = "file://"+currentEditor.folder
            fileDialog.open()
        } else {
            // First check if its open
            for(var i=0; i<stackLayout.count; i++) {
                var editor = stackLayout.itemAt(i)

                // TODO can we replace with Qt.resolvedUrl(filename)
                if(editor.fileUrl == filename) { // == is correct because the string objects aren't identical, just equal strings
                    currentEditor.errorLine = errorLine
                    stackLayout.currentIndex = i
                    return
                }
            }

            if(currentEditor && currentEditor.fileName === "untitled" && currentEditor.text === "") {
                currentEditor.open(filename)
                updateOpenFiles()
                return
            }

            // Nope. Not open, so open in a new tab instead
            var codeEditorComponent = Qt.createComponent("CodeEditor.qml")
            var newCodeEditor = codeEditorComponent.createObject(stackLayout, {errorLine: errorLine });
            newCodeEditor.open(filename)
            newCodeEditor.changedSinceLastSave = false
            stackLayout.currentIndex = fileListView.count - 1
            updateOpenFiles()
        }
    }

    Settings {
        property alias lastOpenedFolder: root.lastOpenedFolder
        property alias openFiles: root.openFiles
        property alias fileListViewWidth: fileListView.width
        property alias stackLayoutWidth: stackLayout.width
    }

    CodeEditor {
        id: dummyEditor
        visible: false
    }

    QQC1.SplitView {
        anchors.fill: parent
        handleDelegate: Item {
            width: 4
            Rectangle {
                anchors {
                    fill: parent
                    margins: 1
                }
                color: Material.color(Material.Grey, Material.Shade900)
            }
        }

        Flickable {
            id: fileListRoot
            onWidthChanged: {
                if(hoverResize) return
                minimumWidth = 50
                maximumWidth = 160
            }

            Component.onCompleted: {
                minimumWidth = 160
                maximumWidth = minimumWidth
                // Hack to force initial size. @dragly? :P
            }

            property real oldWidth
            property bool hoverResize: false
            property bool hovered: false
            property real minimumWidth: 50
            property real maximumWidth: 160
            property real preferredWidth: 160
            Layout.maximumWidth: maximumWidth
            Layout.minimumWidth: minimumWidth
            Layout.preferredWidth: preferredWidth
            Layout.fillHeight: true
            ScrollBar.vertical: ScrollBar {}
            ScrollBar.horizontal: ScrollBar {}
            contentHeight: fileColumn.height
            clip: true

            onHoveredChanged:  {
                hoverResize = true
                if(hovered) {
                    oldWidth = fileListRoot.width
                    fileListRoot.minimumWidth = 160
                } else {
                    fileListRoot.minimumWidth = oldWidth
                    fileListRoot.maximumWidth = oldWidth
                    fileListRoot.maximumWidth = 160
                }
                hoverResize = false
            }

            Column {
                id: fileColumn
                anchors {
                    left: parent.left
                    right: parent.right
                }

                Repeater {
                    id: fileListView
                    model: stackLayout.count

                    ItemDelegate {
                        property CodeEditor codeEditor: stackLayout.itemAt(index)
                        anchors {
                            left: parent ? parent.left : undefined
                            right: parent ? parent.right : undefined
                            rightMargin: 8
                        }

                        highlighted: index === stackLayout.currentIndex
                        height: 32
                        text: codeEditor.title
                        onClicked: {
                            stackLayout.currentIndex = index
                        }
                        onHoveredChanged: fileListRoot.hovered = hovered
                    }
                }

                ItemDelegate {
                    anchors {
                        left: parent.left
                        right: parent.right
                    }

                    height: 32

                    Label { }
                    onHoveredChanged: fileListRoot.hovered = hovered
                }

                ItemDelegate {
                    anchors {
                        left: parent.left
                        right: parent.right
                    }

                    height: 32

                    Label {
                        anchors.centerIn: parent
                        text: parent.width < 80 ? "New" : "New ("+EventCenter.nativeText("editor.new")+")"
                    }

                    onClicked: {
                        newTab()
                    }
                    onHoveredChanged: fileListRoot.hovered = hovered
                }

                ItemDelegate {
                    anchors {
                        left: parent.left
                        right: parent.right
                    }

                    height: 32

                    Label {
                        anchors.centerIn: parent
                        text: parent.width < 80 ? "Open" : "Open ("+EventCenter.nativeText("editor.open")+")"
                    }

                    onClicked: {
                        openTab()
                    }
                    onHoveredChanged: fileListRoot.hovered = hovered
                }

                ItemDelegate {
                    anchors {
                        left: parent.left
                        right: parent.right
                    }

                    height: 32

                    Label {
                        anchors.centerIn: parent
                        text: parent.width < 80 ? "Close" : "Close ("+EventCenter.nativeText("editor.close")+")"
                    }

                    onClicked: {
                        closeTab()
                    }
                    onHoveredChanged: fileListRoot.hovered = hovered
                }

                ItemDelegate {
                    anchors {
                        left: parent.left
                        right: parent.right
                    }
                    visible: parent.width < 80

                    height: 32

                    Label {
                        anchors.centerIn: parent
                        text: ">>"
                    }

                    onClicked: {
                        fileListRoot.minimumWidth = 160
                        fileListRoot.minimumWidth = 50
                        fileListRoot.maximumWidth = 160
                    }
                }
            }
        }

        StackLayout  {
            id: stackLayout
            Layout.fillWidth: true
            Layout.fillHeight: true
            onCountChanged: {
                updateOpenFiles()
            }
        }
    }

    FileDialog {
        id: fileDialog
        selectExisting : true
        selectMultiple: true
        property var cb
        title: "Please choose a file"

        onAccepted: {
            root.lastOpenedFolder = fileDialog.folder
            cb()
            cb = undefined
        }
    }

    MessageDialog {
        id: messageDialog
        property var cb
        title: "Save Changes"
        text: "Do you want to save the changes you made to "
        standardButtons: StandardButton.Save  | StandardButton.Discard | StandardButton.Cancel

        onAccepted: {
            currentEditor.save(function() {
                console.log("Save dialog callback and cancel: ", currentEditor.cancelCloseEditor)
                if(!currentEditor.cancelCloseEditor) {
                    closeTab()
                }
                currentEditor.cancelCloseEditor = false
            })
        }

        onDiscard: {
            currentEditor.changedSinceLastSave = false
            closeTab()
        }
    }

    MessageDialog {
        id: resetScriptDialog
        title: "Cancel current simulation?"
        text: "You have a simulation running. To run a new script you have to cancel the current simulation."
        standardButtons: StandardButton.No | StandardButton.Yes
        onYes: {
            simulator.reset()
            simulator.didReset.connect( function() {
                runScript()
            })
        }
    }

    MessageDialog {
        id: saveChangesDialog
        title: "Save changes?"
        text: "The current file '"+ (currentEditor ? currentEditor.fileName : "")+"' has unsaved changes. "
        standardButtons: StandardButton.Cancel | StandardButton.Save
        onAccepted: {
            currentEditor.save(runScript)
        }
    }

    EventCatcher {
        name: "editor.new"
        onTriggered: {
            root.newTab()
        }
    }

    EventCatcher {
        name: "editor.open"
        onTriggered: {
            root.openTab(data)
        }
    }

    EventCatcher {
        name: "editor.run"
        onTriggered: {
            root.runScript()
        }
    }

    EventCatcher {
        name: "editor.close"
        onTriggered: {
            root.closeTab()
        }
    }

    EventCatcher {
        name: "editor.save"
        onTriggered: {
            currentEditor.save()
        }
    }

    EventCatcher {
        name: "editor.saveAs"
        onTriggered: {
            EventCenter.postEvent("mode.edit")
            currentEditor.saveAs()
        }
    }

    EventMapper {
        mapping: {
             "editor.textArea": currentEditor ? currentEditor.textArea : null,
             "editorWindow": root
        }
    }

    Item {
        id: shortcuts

        property string tabShortcutModifier: Qt.platform.os === "osx" ? "Alt" : "Alt"

        Shortcut {
            sequence: shortcuts.tabShortcutModifier + "+1"
            onActivated: {
                if(editorCount >= 1) stackLayout.currentIndex = 0
            }
        }
        Shortcut {
            sequence: shortcuts.tabShortcutModifier + "+2"
            onActivated: {
                if(editorCount >= 2) stackLayout.currentIndex = 1
            }
        }
        Shortcut {
            sequence: shortcuts.tabShortcutModifier + "+3"
            onActivated: {
                if(editorCount >= 3) stackLayout.currentIndex = 2
            }
        }
        Shortcut {
            sequence: shortcuts.tabShortcutModifier + "+4"
            onActivated: {
                if(editorCount >= 4) stackLayout.currentIndex = 3
            }
        }
        Shortcut {
            sequence: shortcuts.tabShortcutModifier + "+5"
            onActivated: {
                if(editorCount >= 5) stackLayout.currentIndex = 4
            }
        }
        Shortcut {
            sequence: shortcuts.tabShortcutModifier + "+6"
            onActivated: {
                if(editorCount >= 6) stackLayout.currentIndex = 5
            }
        }
        Shortcut {
            sequence: shortcuts.tabShortcutModifier + "+7"
            onActivated: {
                if(editorCount >= 7) stackLayout.currentIndex = 6
            }
        }
        Shortcut {
            sequence: shortcuts.tabShortcutModifier + "+8"
            onActivated: {
                if(editorCount >= 8) stackLayout.currentIndex = 7
            }
        }
//        Shortcut {
//            sequence: shortcuts.tabShortcutModifier + "+9"
//            onActivated: {
//                if(editorCount >= 9) stackLayout.currentIndex = 8
//            }
//        }
//        Shortcut {
//            sequence: shortcuts.tabShortcutModifier + "+0"
//            onActivated: {
//                if(editorCount >= 10) stackLayout.currentIndex = 9
//            }
//        }
        Shortcut {
            sequence: shortcuts.tabShortcutModifier + "+Left"
            onActivated: {
                if(stackLayout.currentIndex > 0) stackLayout.currentIndex -= 1
                else stackLayout.currentIndex = editorCount-1
            }
        }
        Shortcut {
            sequence: shortcuts.tabShortcutModifier + "+Right"
            onActivated: {
                if(stackLayout.currentIndex < editorCount-1) stackLayout.currentIndex += 1
                else stackLayout.currentIndex = 0
            }
        }
    }

}
