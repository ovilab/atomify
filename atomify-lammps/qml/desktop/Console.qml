import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2
import Atomify 1.0

ColumnLayout {
    id: consoleRoot
    property AtomifySimulator simulator
    property TextArea output: consoleOutput
    Layout.fillHeight: true
    Layout.fillWidth: true
    TextArea {
        id: consoleOutput
        Layout.fillHeight: true
        Layout.fillWidth: true
        Layout.minimumHeight: 100
        readOnly: true
    }

    Row {
        Layout.fillWidth: true

        TextField {
            property int previousCommandCounter: 0
            id: singleCommand
            width: parent.width - runSingleCommand.width

            Keys.onPressed: {
                if(singleCommand.text == "") {
                    simulator.scriptHandler.lastSingleCommand();
                }
            }

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
                onActivated: {
                    if(singleCommand.text == "") {
                        singleCommand.text = simulator.scriptHandler.lastSingleCommand();
                    } else {
                        singleCommand.text = simulator.scriptHandler.previousSingleCommand();
                    }
                }
            }
            Shortcut {
                sequence: "Down"
                onActivated: {
                    singleCommand.text = simulator.scriptHandler.nextSingleCommand();
                }
            }
        }

        Button {
            id: runSingleCommand
            text: "Run"
            onClicked: {
                simulator.scriptHandler.runCommand(singleCommand.text, true)
                consoleOutput.append(singleCommand.text)
                singleCommand.text = ""
            }
        }
    }
}
