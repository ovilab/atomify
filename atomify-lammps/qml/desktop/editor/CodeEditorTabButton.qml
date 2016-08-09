import QtQuick 2.7
import QtQuick.Controls 2.0

TabButton {
    id: root
    property CodeEditor codeEditor
    property alias color: rect.color
    height: parent ? parent.height : 0
    onCodeEditorChanged: {
        if(codeEditor === undefined || codeEditor === null) return
        if(codeEditor != undefined) {
            text = codeEditor.title
        }

        codeEditor.titleChanged.connect(function() {
            text = codeEditor.title
        })
    }

    background: Rectangle {
        id: rect
        width: parent.width
        height: parent.height
    }

    onFocusChanged: {
        rect.color = root.focus ? "#fff" : "#aaa"
    }

    implicitWidth: width
    width: parent ? parent.width / parent.count : 0
}
