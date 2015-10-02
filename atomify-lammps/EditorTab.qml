import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2
import MySimulator 1.0

Item {
    id: editorTabRoot
    property TextArea consoleOutput: myConsole.output
    property LammpsEditor lammpsEditor: myLammpsEditor
    property MySimulator simulator

    SplitView {
        orientation: Qt.Vertical
        anchors.fill: parent

        LammpsEditor {
            id: myLammpsEditor
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.minimumHeight: 200
            Layout.preferredHeight: parent.height*0.75
            simulator: mySimulator

            Shortcut {
                sequence: "Escape"
                onActivated: {
                    if(myLammpsEditor.textarea.focus) myLammpsEditor.textarea.focus = false
                    else mySimulator.paused = !mySimulator.paused
                }
            }
        }

        Console {
            id: myConsole
            simulator: editorTabRoot.simulator
        }
    }
}
