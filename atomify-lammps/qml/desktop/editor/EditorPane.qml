import QtQuick 2.7
import QtQuick.Controls 1.5 as QQC1
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.2
import Atomify 1.0
import SimVis 1.0
import "../../visualization"
Pane {
    id: editorTabRoot
    property TextArea consoleOutput: consoleItem.output
    property alias editorWindow: editorWindow
    property alias automaticallyRun: automaticallyRun.checked
    property AtomifySimulator simulator
    property AtomifyVisualizer visualizer
    onConsoleOutputChanged: {
        consoleOutput.onLinkActivated.connect(function(link){
            var params = editorWindow.currentEditor.getParameters(link)
            var errorLine = params["errorLine"]
            console.log("Link before: ", link)
            link = editorWindow.dummyEditor.cleanPath(link)
            console.log("Link after: ", link)
            editorWindow.codeEditorWindow.openTab(link, errorLine)
        })
    }

    function reportError() {
        if(simulator.lammpsError) {
            if(simulator.lammpsError.scriptFile === "") {
                consoleOutput.append(" Simulation crashed on line "+simulator.lammpsError.line)
                consoleOutput.append(" Command: '"+simulator.lammpsError.command+"'")
                consoleOutput.append(" Error: '"+simulator.lammpsError.message+"'")
                editorWindow.errorLine = simulator.scriptHandler.currentLine
            } else {
                consoleOutput.append(" Simulation crashed.")
                consoleOutput.append(" File: <a style=\"color: #56B6C2; font-weight: bold;\" href=\"file://"+simulator.lammpsError.scriptPath + "?errorLine="+simulator.lammpsError.line+"\">"+simulator.lammpsError.scriptFile+"</a> on line " + simulator.lammpsError.line)
                consoleOutput.append(" Command: '"+simulator.lammpsError.command+"'")
                consoleOutput.append(" Error: '"+simulator.lammpsError.message+"'")
                editorWindow.errorLine = simulator.scriptHandler.currentLine
            }
        }
    }

    ColumnLayout {
        anchors.fill: parent

        CodeEditorWindow {
            id: editorWindow
            simulator: editorTabRoot.simulator
            Layout.fillHeight: true
            Layout.fillWidth: true
            onClearConsole: consoleItem.clear()
            onDidRun: visualizer.focus = true
        }

        RowLayout {
            anchors {
                right: parent.right
            }

            CheckBox {
                id: automaticallyRun
                focusPolicy: Qt.NoFocus
                text: "Automatically run"
            }

            Button {
                id: clear
                text: "Clear output"
                onClicked: {
                    consoleItem.clear()
                }
            }
            Button {
                id: runButton
                anchors {
                    right: parent.right
                }
                text: "Run script"
                onClicked: {
                    editorWindow.runScript()
                }
            }
        }

        Console {
            id: consoleItem
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.maximumHeight: 200
            simulator: editorTabRoot.simulator
        }
    }

    Shortcut {
        sequence: "Escape"
        onActivated: {
            visualizer.focus = true
        }
    }
}
