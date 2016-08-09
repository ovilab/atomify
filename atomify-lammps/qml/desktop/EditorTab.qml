import QtQuick 2.7
import QtQuick.Controls 1.5 as QQC1
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.2
import Atomify 1.0
import SimVis 1.0
import "../visualization"
Pane {
    id: editorTabRoot
    property TextArea consoleOutput: consoleItem.output
    property alias lammpsEditor: lammpsEditor
    property AtomifySimulator simulator
    property AtomifyVisualizer visualizer
    onConsoleOutputChanged: {
        consoleOutput.onLinkActivated.connect(function(link){
            lammpsEditor.codeEditorWindow.openTab(link)
        })
    }

    function reportError() {
        if(simulator.lammpsError) {
            if(simulator.lammpsError.scriptFile === "") {
                consoleOutput.append(" Simulation crashed on line "+simulator.lammpsError.line)
                consoleOutput.append(" Command: '"+simulator.lammpsError.command+"'")
                consoleOutput.append(" Error: '"+simulator.lammpsError.message+"'")
                lammpsEditor.codeEditorWindow.errorLine = simulator.lammpsError.line
            } else {
                consoleOutput.append(" Simulation crashed.")
                consoleOutput.append(" File: <a href=\"file://" + simulator.lammpsError.scriptFile + "\">"+simulator.lammpsError.scriptFile+"</a> on line " + simulator.lammpsError.line)
                consoleOutput.append(" Command: '"+simulator.lammpsError.command+"'")
                consoleOutput.append(" Error: '"+simulator.lammpsError.message+"'")
            }
        }
    }

    ColumnLayout {
        anchors.fill: parent

        LammpsEditor {
            id: lammpsEditor
            simulator: editorTabRoot.simulator
            Layout.fillHeight: true
            Layout.fillWidth: true
            onClearConsole: consoleItem.clear()
        }

        RowLayout {
            anchors {
                right: parent.right
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
                    lammpsEditor.runScript()
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
