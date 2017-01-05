import QtQuick 2.7
import QtQuick.Controls 2.0
import Qt.labs.settings 1.0
//import QtQuick.Controls 1.5
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.2
import Atomify 1.0

import "../.."

Item {
    id: root
    property alias dummyEditor: dummyEditor
    property AtomifySimulator simulator
    property CodeEditor currentEditor: (stackLayout.currentIndex==-1) ? null : stackLayout.itemAt(stackLayout.currentIndex)
    property CodeEditor activeEditor
    property CodeEditorTabButton currentTabButton: (tabBar.currentIndex==-1) ? null : tabBar.itemAt(tabBar.currentIndex)
    property alias editorCount: stackLayout.count
    property int currentLine: -1
    property int errorLine: -1
    property string openFiles: ""
    property real lastRunScript: 0
    property string lastOpenedFolder
    onVisibleChanged: {
        if(visible) {
            if(visualizer.simulator.scriptFilePath!=="") {
                openTab("file://"+visualizer.simulator.scriptFilePath)
            }
        }
    }

    clip: true
    signal clearConsole()
    signal didRun()

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

    Settings {
        property alias lastOpenedFolder: root.lastOpenedFolder
        property alias openFiles: root.openFiles
    }

    CodeEditor {
        id: dummyEditor
        visible: false

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
        if(currentEditor) currentEditor.textArea.focus = true
    }

    function newTab() {
        var newCodeEditor = Qt.createQmlObject("import QtQuick 2.7; CodeEditor { }", stackLayout);
        var newTabButton = Qt.createQmlObject("import QtQuick 2.7; import QtQuick.Controls 2.0; CodeEditorTabButton { }", tabBar);
        newTabButton.codeEditor = newCodeEditor
        newCodeEditor.changedSinceLastSave = false
        tabBar.setCurrentIndex(tabBar.count-1) // select it
        focusCurrentEditor()

        updateOpenFiles()
    }

    function showDoYouWantToSave(fileName) {
        messageDialog.text = "Do you want to save the changes you made to "+fileName+"?"
        messageDialog.visible = true
    }

    function closeTab() {
        if(currentEditor === null) return;

        if(currentEditor.changedSinceLastSave) {
            // Ask user to save the file before we close the tab
            messageDialog.cb = function() {
                // Callback is to close the tab
                var indexOfCurrentTab = stackLayout.currentIndex
                var editor = currentEditor
                currentTabButton.codeEditor = null
                currentEditor.parent = null
                editor.destroy()
                tabBar.removeItem(indexOfCurrentTab)
                messageDialog.cb = null
            }

            showDoYouWantToSave(currentEditor.fileName)
        } else {
            var indexOfCurrentTab = stackLayout.currentIndex
            var editor = currentEditor

            currentTabButton.codeEditor = null
            currentEditor.parent = null
            editor.destroy()
            tabBar.removeItem(indexOfCurrentTab)
            messageDialog.cb = null
        }

        if(editorCount == 0) {
            newTab()
        }

        if(indexOfCurrentTab >= editorCount) {
            indexOfCurrentTab -= 1
        }

        tabBar.currentIndex = indexOfCurrentTab

        updateOpenFiles()
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

                if(editor.fileUrl==filename) { // == is correct because the string objects aren't identical, just equal strings
                    tabBar.currentIndex = i
                    currentEditor.errorLine = errorLine
                    return
                }
            }

            if(currentEditor && currentEditor.fileName === "untitled" && currentEditor.text === "") {
                currentEditor.open(filename)
                updateOpenFiles()
                return
            }

            // Nope. Not open, so open in a new tab instead
            var newCodeEditor = Qt.createQmlObject("import QtQuick 2.7; CodeEditor { errorLine: "+errorLine+" }", stackLayout);
            var newTabButton = Qt.createQmlObject("import QtQuick 2.7; import QtQuick.Controls 2.0; CodeEditorTabButton { }", tabBar);
            newTabButton.codeEditor = newCodeEditor
            newCodeEditor.open(filename)
            newCodeEditor.changedSinceLastSave = false
            tabBar.setCurrentIndex(tabBar.count-1)
            updateOpenFiles()
        }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0
        TabBar {
            id: tabBar
            anchors {
                left: parent.left
                right: parent.right
            }
        }

        SwipeView {
            id: stackLayout
            Layout.fillWidth: true
            Layout.fillHeight: true
            currentIndex: tabBar.currentIndex
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

    EventMapper {
        mapping: {
             "editor.textArea": currentEditor ? currentEditor.textArea : null,
             "editor": currentEditor,
             "editorWindow": root
        }
    }

    Item {
        id: shortcuts

        property string tabShortcutModifier: Qt.platform.os === "osx" ? "Alt" : "Alt"

        Shortcut {
            sequence: shortcuts.tabShortcutModifier + "+1"
            onActivated: {
                if(editorCount >= 1) tabBar.setCurrentIndex(0)
            }
        }
        Shortcut {
            sequence: shortcuts.tabShortcutModifier + "+2"
            onActivated: {
                if(editorCount >= 2) tabBar.setCurrentIndex(1)
            }
        }
        Shortcut {
            sequence: shortcuts.tabShortcutModifier + "+3"
            onActivated: {
                if(editorCount >= 3) tabBar.setCurrentIndex(2)
            }
        }
        Shortcut {
            sequence: shortcuts.tabShortcutModifier + "+4"
            onActivated: {
                if(editorCount >= 4) tabBar.setCurrentIndex(3)
            }
        }
        Shortcut {
            sequence: shortcuts.tabShortcutModifier + "+5"
            onActivated: {
                if(editorCount >= 5) tabBar.setCurrentIndex(4)
            }
        }
        Shortcut {
            sequence: shortcuts.tabShortcutModifier + "+6"
            onActivated: {
                if(editorCount >= 6) tabBar.setCurrentIndex(5)
            }
        }
        Shortcut {
            sequence: shortcuts.tabShortcutModifier + "+7"
            onActivated: {
                if(editorCount >= 7) tabBar.setCurrentIndex(6)
            }
        }
        Shortcut {
            sequence: shortcuts.tabShortcutModifier + "+8"
            onActivated: {
                if(editorCount >= 8) tabBar.setCurrentIndex(7)
            }
        }
        Shortcut {
            sequence: shortcuts.tabShortcutModifier + "+9"
            onActivated: {
                if(editorCount >= 9) tabBar.setCurrentIndex(8)
            }
        }
        Shortcut {
            sequence: shortcuts.tabShortcutModifier + "+0"
            onActivated: {
                if(editorCount >= 10) tabBar.setCurrentIndex(9)
            }
        }
        Shortcut {
            sequence: StandardKey.New
            onActivated: {
                newTab()
            }
        }
        Shortcut {
            sequence: StandardKey.AddTab
            onActivated: {
                newTab()
            }
        }
        Shortcut {
            sequence: StandardKey.Open
            onActivated: {
                openTab()
            }
        }

        Shortcut {
            sequence: StandardKey.Save
            onActivated: {
                currentEditor.save()
            }
        }

        Shortcut {
            sequence: StandardKey.Close
            onActivated: {
                closeTab()
            }
        }

        Shortcut {
            sequence: StandardKey.SaveAs
            onActivated: {
                currentEditor.saveAs()
            }
        }
    }

}
