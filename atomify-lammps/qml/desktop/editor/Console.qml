import QtQuick 2.5
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.2
import Atomify 1.0

ColumnLayout {
    id: consoleRoot
    property AtomifySimulator simulator
    property TextArea output: consoleOutput
    property alias textField: singleCommand

    function clear() {
        consoleOutput.text = ""
    }

    Layout.fillHeight: true
    Layout.fillWidth: true
    Flickable {
        id: flickableItem
        Layout.fillHeight: true
        Layout.fillWidth: true
        Layout.minimumHeight: 100
        TextArea.flickable: TextArea {
            id: consoleOutput
            anchors.fill: parent
            textFormat: TextEdit.RichText
            readOnly: true
            placeholderText: "No output yet..."
        }

        ScrollBar.vertical: ScrollBar {
            id: scrollbar
        }
    }

    RowLayout {
        Layout.fillWidth: true

        TextField {
            property int previousCommandCounter: 0
            id: singleCommand
            Layout.fillWidth: true

            Keys.onPressed: {
                if(singleCommand.text == "") {
                    simulator.scriptHandler.lastSingleCommandString();
                }
            }

            placeholderText: "Run single LAMMPS command here"

            Shortcut {
                sequence: "Return"
                onActivated: {
                    if(singleCommand.text != "") {
                        simulator.scriptHandler.runCommand(singleCommand.text, true)
                        consoleOutput.append(singleCommand.text)
                        singleCommand.text = ""
                    }
                }
            }
            Shortcut {
                sequence: "Up"
                enabled: singleCommand.activeFocus
                onActivated: {
                    if(singleCommand.text == "") {
                        singleCommand.text = simulator.scriptHandler.lastSingleCommandString();
                    } else {
                        singleCommand.text = simulator.scriptHandler.previousSingleCommandString();
                    }
                }
            }
            Shortcut {
                sequence: "Down"
                enabled: singleCommand.activeFocus
                onActivated: {
                    singleCommand.text = simulator.scriptHandler.nextSingleCommandString();
                }
            }
        }
    }
}
