import QtQuick 2.5
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.1
import Atomify 1.0
import Qt.labs.settings 1.0

Item {
    id: lammpsEditorRoot

    signal clearConsole()

    property AtomifySimulator simulator
    property alias codeEditorWindow: codeEditorWindow

    onSimulatorChanged: {
        simulator.willResetChanged.connect(function() {
            codeEditorWindow.clear()
        })
    }

    function runScript() {
        if(!simulator.scriptHandler) {
            return
        }
        simulator.willReset = true
        simulator.scriptHandler.reset()
        simulator.scriptHandler.setWorkingDirectory(codeEditorWindow.currentEditor.fileUrl)
        simulator.scriptHandler.runScript(codeEditorWindow.currentEditor.text)
        codeEditorWindow.activeEditor = codeEditorWindow.currentEditor
    }

    CodeEditorWindow {
        id: codeEditorWindow
        currentLine: simulator.scriptHandler.currentLine
        anchors.fill: parent
    }
}
