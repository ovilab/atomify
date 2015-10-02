import QtQuick 2.4
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import Highlighter 1.0
import MySimulator 1.0
import Qt.labs.settings 1.0

Item {
    id: lammpsEditorRoot

    property MySimulator simulator
    property TextArea textarea: textarea
    function runScript() {
        simulator.willReset = true
        simulator.scriptHandler.reset()
        simulator.scriptHandler.runScript(textarea.text)
    }

    Component.onCompleted: {
        highlighter.setTextDocument(textarea.textDocument)
    }

    Highlighter {
        id: highlighter
    }

    ColumnLayout {
        spacing: 2
        anchors.fill: parent

        TextArea {
            id: textarea
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignTop
            Settings {
                property alias text: textarea.text
            }
            text: "variable L equal 12.0
variable thickness equal 3.0

units lj
atom_style atomic
atom_modify map hash
lattice fcc 0.8442

variable left equal 0.5*$L-0.5*${thickness}
variable right equal 0.5*$L+0.5*${thickness}
region system block 0 $L 0 $L 0 $L
region boxinside block ${left} ${right} 0 $L 0 $L
region boxoutside block ${left} ${right} 0 $L 0 $L side out
region corner block 0 3 0 3 0 3

create_box 3 system
create_atoms 1 region boxinside
create_atoms 2 region boxoutside
delete_atoms region corner
create_atoms 3 region corner
mass * 1.0

velocity all create 3.44 87287 loop geom

pair_style lj/cut 2.5
pair_coeff * * 1.0 1.0 2.5

neighbor 0.3 bin
neigh_modify delay 0 every 20 check no

fix 1 all nve"
        }

        RowLayout {
            spacing: 2
            Layout.alignment: Qt.AlignBottom

            Button {
                id: runButton
                Layout.alignment: Qt.AlignCenter
                text: "Run"
                onClicked: {
                    runScript()
                }
            }

            Button {
                id: pauseButton
                Layout.alignment: Qt.AlignCenter
                text: "Toggle pause"
                onClicked: {
                    lammpsEditorRoot.simulator.setPaused(!lammpsEditorRoot.simulator.paused)
                }
            }

            Slider {
                id: speedSlider
                // anchors.left: runButton.right
                minimumValue: 1
                maximumValue: 20
                stepSize: 1
                value: 1
                onValueChanged: {
                    if(simulator != undefined) {
                        simulator.simulationSpeed = value
                    }
                }
            }
            Label {
                id: speedLabel
                // anchors.left: speedSlider.right
                text: "Simulation speed: "+speedSlider.value
            }
        }

    }
}
