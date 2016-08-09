import QtQuick 2.7
import QtQuick.Controls 2.0

TabButton {
    id: root
    property CodeEditor codeEditor
    font.pixelSize: 12
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

    implicitWidth: width
    width: parent ? parent.width / parent.count : 0
}
