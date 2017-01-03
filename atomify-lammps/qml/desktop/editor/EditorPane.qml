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
    property AtomifySimulator simulator
    property AtomifyVisualizer visualizer
    onConsoleOutputChanged: {
        consoleOutput.onLinkActivated.connect(function(link){
            var params = editorWindow.currentEditor.getParameters(link)
            var errorLine = params["errorLine"]
            link = editorWindow.dummyEditor.cleanPath(link)
            editorWindow.openTab(link, errorLine)
        })
    }

    function reportError() {
        if(simulator.crashed) {
            var error = simulator.error
            consoleOutput.append(" Simulation crashed with message "+error)
//            if(error.scriptFile === "") {
//                consoleOutput.append(" Simulation crashed on line "+error.line)
//                consoleOutput.append(" Command: '"+error.command+"'")
//                consoleOutput.append(" Error: '"+error.message+"'")
//                editorWindow.errorLine = error.line
//            } else {
//                consoleOutput.append(" Simulation crashed.")
//                consoleOutput.append(" File: <a style=\"color: #56B6C2; font-weight: bold;\" href=\"file://"+error.scriptPath + "?errorLine="+error.line+"\">"+error.scriptFile+"</a> on line " + error.line)
//                consoleOutput.append(" Command: '"+error.command+"'")
//                consoleOutput.append(" Error: '"+error.message+"'")
//                editorWindow.errorLine = error.line
//            }
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

            Button {
                id: clear
                text: "Clear output"
                focusPolicy: Qt.NoFocus
                onClicked: {
                    consoleItem.clear()
                }
            }
            Button {
                id: runButton
                focusPolicy: Qt.NoFocus
                anchors {
                    right: parent.right
                }
                text: {
                    if(simulator.states.idle.active || simulator.states.finished.active) {
                        return "Run script"
                    } else {
                        return "Stop script"
                    }
                }

                enabled: !simulator.states.reset.active // Waiting to reset...

                onClicked: {
                    if(simulator.states.finished.active) {
                        editorWindow.runScript()
                    } else if(!simulator.states.idle.active) {
                        simulator.reset()
                    } else {
                        editorWindow.runScript()
                    }
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
        sequence: "Ctrl+R"
        context: Qt.ApplicationShortcut
        onActivated: editorWindow.runScript()
    }
}
