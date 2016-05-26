import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2
import Atomify 1.0
import SimVis 1.0
import "../visualization"
Item {
    id: editorTabRoot
    property TextArea consoleOutput: myConsole.output
    property LammpsEditor lammpsEditor: myLammpsEditor
    property AtomifySimulator simulator
    property AtomifyVisualizer visualizer

    onVisualizerChanged: {
        console.log("EditorTabVisChanged: ", visualizer)
    }

    onSimulatorChanged: {
        console.log("EditorTabSimChanged: ", simulator)
    }


    function reportError() {
//        consoleOutput.append(" Simulation crashed. Error in parsing LAMMPS command: '" + simulator.scriptHandler.currentCommand + "'")
        consoleOutput.append(" LAMMPS error message: '" + simulator.lammpsErrorMessage + "'")
    }

    SplitView {
        orientation: Qt.Vertical
        anchors.fill: parent

        LammpsEditor {
            id: myLammpsEditor
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.minimumHeight: 200
            Layout.preferredHeight: parent.height*0.75
            simulator: editorTabRoot.simulator

//            Shortcut {
//                sequence: "Escape"
//                onActivated: {
//                    if(myLammpsEditor.textarea.focus) {
//                        myLammpsEditor.textarea.focus = false
//                        console.log("Visualizer: "+visualizer)
//                        visualizer.focus = true
//                    } else {
//                        editorTabRoot.paused = !editorTabRoot.paused
//                    }
//                }
//            }
        }

        Console {
            id: myConsole
            simulator: editorTabRoot.simulator
        }
    }

    Shortcut {
        sequence: "Escape"
        onActivated: {
            if(myLammpsEditor.textarea.activeFocus || myConsole.textField.activeFocus) {
                myLammpsEditor.textarea.focus = false
                myConsole.textField.focus = false
                visualizer.focus = true
            } else {
                editorTabRoot.paused = !editorTabRoot.paused
            }
        }
    }
}
