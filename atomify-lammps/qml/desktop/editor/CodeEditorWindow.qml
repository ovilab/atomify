import QtQuick 2.7
import QtQuick.Controls 2.0
import Qt.labs.settings 1.0
//import QtQuick.Controls 1.5
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.2

Item {
    property CodeEditor currentEditor: (stackLayout.currentIndex==-1) ? null : stackLayout.itemAt(stackLayout.currentIndex)
    property CodeEditorTabButton currentTabButton: (tabBar.currentIndex==-1) ? null : tabBar.itemAt(tabBar.currentIndex)
    property alias editorCount: stackLayout.count
    property int currentLine: -1
    property int errorLine: -1
    onCurrentLineChanged: {
        if(currentEditor != undefined) currentEditor.currentLine = currentLine
    }

    onErrorLineChanged: {
        if(currentEditor != undefined) currentEditor.errorLine = errorLine
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
    }

    function openTab(filename) {
        if(filename === undefined) {
            fileDialog.cb = function() {
                if(currentEditor.title === "untitled" && currentEditor.text === "") {
                    currentEditor.open(fileDialog.fileUrl)
                } else {
                    var newCodeEditor = Qt.createQmlObject("import QtQuick 2.7; CodeEditor { }", stackLayout);
                    var newTabButton = Qt.createQmlObject("import QtQuick 2.7; import QtQuick.Controls 2.0; CodeEditorTabButton { }", tabBar);
                    newTabButton.codeEditor = newCodeEditor
                    newCodeEditor.open(fileDialog.fileUrl)
                    newCodeEditor.changedSinceLastSave = false
                    tabBar.setCurrentIndex(tabBar.count-1)
                    newTabButton.color = "#fff" // Hack since focus isn't set correctly when it's the first tab?
                    focusCurrentEditor()
                }

            }
            fileDialog.visible = true
        } else {
            // First check if its open
            for(var i=0; i<stackLayout.count; i++) {
                var editor = stackLayout.itemAt(i)

                if(editor.fileUrl==filename) {
                    tabBar.currentIndex = i
                    return;
                }
            }

            // Nope. Not open, so open in a new tab instead
            var newCodeEditor = Qt.createQmlObject("import QtQuick 2.7; CodeEditor { }", stackLayout);
            var newTabButton = Qt.createQmlObject("import QtQuick 2.7; import QtQuick.Controls 2.0; CodeEditorTabButton { }", tabBar);
            newTabButton.codeEditor = newCodeEditor
            newCodeEditor.open(filename)
            newCodeEditor.changedSinceLastSave = false
            tabBar.setCurrentIndex(tabBar.count-1)
            newTabButton.color = "#fff" // Hack since focus isn't set correctly when it's the first tab?
        }
    }

    Settings {
        property alias lastOpenedFolder: fileDialog.folder
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

            CodeEditorTabButton {
                text: codeEditor_1.title
                codeEditor: codeEditor_1
            }
        }

        StackLayout {
            id: stackLayout
            Layout.fillWidth: true
            Layout.fillHeight: true
            currentIndex: tabBar.currentIndex

            CodeEditor {
                id: codeEditor_1
                Component.onCompleted: {
                    changedSinceLastSave = false
                }
            }
        }
    }

    FileDialog {
        id: fileDialog
        selectExisting : true
        property var cb
        title: "Please choose a file"

        onAccepted: {
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

    Item {
        id: shortcuts

        property string tabShortcutModifier: Qt.platform.os === "osx" ? "Ctrl" : "Alt"

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
