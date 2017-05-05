import QtQuick 2.7
import QtQuick.Controls 2.1

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
    width: parent ? Math.min(140, parent.width / 4) : 140
    implicitWidth: width
}
