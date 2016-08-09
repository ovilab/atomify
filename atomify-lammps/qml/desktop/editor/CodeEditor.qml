import QtQuick 2.7
import QtQuick.Controls 2.0

import QtQuick.Dialogs 1.2
import Atomify 1.0

Item {
    id: root
    property alias text: textArea.text
    property alias textArea: textArea
    property string title: changedSinceLastSave ? fileName+"*" : fileName
    property alias fileName: backend.fileName
    property alias fileUrl: backend.fileUrl
    property bool changedSinceLastSave: false
    property int currentLine: -1
    property int errorLine: -1

    clip: true

    onCurrentLineChanged: {
        lineNumbers.currentLine = currentLine
        textArea.update()
    }

    onErrorLineChanged: {
        lineNumbers.errorLine = errorLine
        textArea.update()
    }

    function clear() {
        currentLine = -1
        errorLine = -1
        textArea.update()
    }

    function open(fileUrl) {
        backend.fileUrl = fileUrl
        backend.load()
        textArea.text = backend.text
        changedSinceLastSave = false
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
                if(cb != undefined) cb()
            }
        }
    }

    function saveAs() {
        backend.text = textArea.text
        fileDialogSave.cb = function() {
            save()
            fileDialogSave.cb = undefined
        }
        fileDialogSave.visible = true
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
        fileName: "untitled"
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

        TextArea.flickable: TextArea {
            id: textArea
            clip: true
            wrapMode: TextArea.NoWrap
            selectByMouse: true
            selectByKeyboard: true
            font.family: "DejaVu Sans Mono"
            font.pixelSize: 12

            text: "variable L equal 12.0
variable thickness equal 3.0

units lj
atom_style atomic
atom_modify map hash
lattice fcc 0.8442

variable left equal 0.5*$L-0.5*${thickness}
variable right equal 0.5*$L+0.5*${thickness}
region system block 0 $L 0 $L 0 $L
region boxinside block ${left} ${right} 0 $L 0 $L
region boxoutside block ${left} ${right} 0 $L 0 $L side out
region corner block 0 3 0 3 0 3

create_box 3 system
create_atoms 1 region boxinside
create_atoms 2 region boxoutside
delete_atoms region corner
create_atoms 3 region corner
mass * 1.0

velocity all create 3.44 87287 loop geom

pair_style lj/cut 2.5
pair_coeff * * 1.0 1.0 2.5

neighbor 0.3 bin
neigh_modify delay 0 every 20 check no

fix 1 all nve
run 100
run 50
run 50
run 50
run 50
run 50
run 100
"

            Component.onCompleted: {
                flickableItem.contentYChanged.connect(update)
                update()
            }

            onTextChanged: {
                update()
            }

            function update() {
                var lineHeight = (contentHeight-textMargin) / lineCount
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
        ScrollBar.vertical: ScrollBar {}

    }

    FileDialog {
        id: fileDialogSave
        selectExisting : false
        property var cb
        title: "Please choose a location to save"

        onAccepted: {
            backend.fileUrl = fileDialogSave.fileUrl
            if(cb != undefined) {
                cb()
                cb = null
            }
        }
    }

    FileDialog {
        id: fileDialogLoad
        selectExisting : true
        property var cb
        title: "Please choose a file"

        onAccepted: {
            backend.fileUrl = fileDialogSave.fileUrl
            if(cb != undefined) {
                cb()
                cancelCloseEditor = false
                cb = null
            }
        }
        onRejected: {
            cancelCloseEditor = true
            cb = null
        }
    }
}
