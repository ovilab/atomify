import QtQuick 2.7
import QtQuick.Controls 2.1

import QtQuick.Dialogs 1.2
import Atomify 1.0

Item {
    id: root
    clip: true

    property alias text: textArea.text
    property alias textArea: textArea
    property string title: changedSinceLastSave ? fileName+"*" : fileName
    property alias fileName: backend.fileName
    property alias fileUrl: backend.fileUrl
    property alias folder: backend.folder
    property bool changedSinceLastSave: false
    property int currentLine: -1
    property int errorLine: -1

    onCurrentLineChanged: {
        lineNumbers.currentLine = currentLine
        textArea.update()
    }

    onErrorLineChanged: {
        lineNumbers.errorLine = errorLine
        textArea.update()
    }

    function loadAndUpdateTextField() {
        backend.load()
        textArea.text = backend.text
        changedSinceLastSave = false
    }

    function refresh() {
        if(backend.fileUrl=="") return

        if(backend.anyChangesOnDisk()) {
            if(changedSinceLastSave) {
                loadChangedFileDialog.open()
                return
            }
            loadAndUpdateTextField()
        }
    }

    function fileExists(path) {
        return backend.fileExists(path);
    }

    function cleanPath(path) {
        return backend.cleanPath(path);
    }

    function getParameters(path) {
        return backend.getParameters(path);
    }


    function clear() {
        currentLine = -1
        errorLine = -1
        textArea.update()
    }

    function open(fileUrl) {
        backend.fileUrl = fileUrl
        loadAndUpdateTextField()
    }

    function save(cb) {
        backend.text = textArea.text
        if(fileName === "untitled") {
            fileDialogSave.cb = function() {
                save(cb)
                fileDialogSave.cb = undefined
            }
            fileDialogSave.visible = true
        } else {
            if(backend.save()) {
                changedSinceLastSave = false
                if(cb !== undefined) cb()
            }
        }
    }

    function saveAs() {
        backend.text = textArea.text
        fileDialogSave.cb = function() {
            save()
            fileDialogSave.cb = undefined
        }
        fileDialogSave.folder = "file://"+root.folder
        fileDialogSave.open()
    }

    Component.onCompleted: {
        highlighter.setTextDocument(textArea.textDocument)
    }

    Highlighter {
        id: highlighter
    }

    LineNumbers {
        id: lineNumbers
        anchors {
            top: parent.top
            bottom: parent.bottom
            margins: 16
        }
        width: 30

        color: "#777"
        selectedColor: "#ddd"
        errorColor: "#e77"
        activeColor: "#7e7"
        font: textArea.font
    }

    CodeEditorBackend {
        id: backend
    }

    Flickable {
        id: flickableItem
        anchors {
            top: lineNumbers.top
            topMargin: -8
            left: lineNumbers.right
            leftMargin: 16
            right: parent.right
            bottom: parent.bottom
        }
        clip: true
        ScrollBar.vertical: ScrollBar {}
        ScrollBar.horizontal: ScrollBar {}

        TextArea.flickable: TextArea {
            id: textArea
            clip: true
            wrapMode: TextArea.NoWrap
            selectByMouse: true
            selectByKeyboard: true
            font.family: "DejaVu Sans Mono"
            font.pixelSize: 12
            enabled: root.visible

            Component.onCompleted: {
                flickableItem.contentYChanged.connect(update)
                update()
            }

            onTextChanged: {
                changedSinceLastSave = true
                update()
            }

            onCursorPositionChanged: {
                update()
            }

            function update() {
                var lineHeight = (contentHeight-2*textMargin) / lineCount
                lineNumbers.lineCount = lineCount
                lineNumbers.scrollY = flickableItem.contentY
                lineNumbers.lineHeight = lineHeight
                lineNumbers.cursorPosition = cursorPosition
                lineNumbers.selectionStart = selectionStart
                lineNumbers.selectionEnd = selectionEnd
                lineNumbers.text = text
                lineNumbers.update()
            }
        }

    }

    FileDialog {
        id: fileDialogSave
        selectExisting : false
        property var cb
        title: "Please choose a location to save"

        onAccepted: {
            backend.fileUrl = fileDialogSave.fileUrl
            if(cb !== undefined) {
                cb()
                cb = null
            }
        }
    }

    MessageDialog {
        id: loadChangedFileDialog
        title: ""

        text: root.fileName+" has changed on disk. Do you want to reload it?"
        standardButtons: StandardButton.No | StandardButton.Yes
        onYes: loadAndUpdateTextField()
    }
}
