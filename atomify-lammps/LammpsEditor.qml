import QtQuick 2.3
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import Highlighter 1.0
import MySimulator 1.0
Item {
    id: lammpsEditorRoot
    property MySimulator simulator

    Component.onCompleted: {
        highlighter.setTextDocument(script.textDocument)
    }

    Highlighter {
        id: highlighter
    }

    ColumnLayout {
        spacing: 2
        anchors.fill: parent

        TextArea {
            id: script
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignTop
            text:
                "# 3d Lennard-Jones melt

variable x index 1
variable y index 1
variable z index 1

variable xx equal 20*$x
variable yy equal 20*$y
variable zz equal 20*$z

units lj
atom_style atomic

lattice fcc 0.8442
region box block 0 ${xx} 0 ${yy} 0 ${zz}
create_box 1 box
create_atoms 1 box
mass 1 1.0

velocity all create 1.44 87287 loop geom

pair_style lj/cut 2.5
pair_coeff 1 1 1.0 1.0 2.5

neighbor 0.3 bin
neigh_modify delay 0 every 20 check no

fix 1 all nve

run 100"
        }

        RowLayout {
            spacing: 2
            Layout.alignment: Qt.AlignBottom

            Button {
                id: runButton
                Layout.alignment: Qt.AlignCenter
                text: "Run"
                onClicked: {
                    console.log("Running script: "+script.text)
                    lammpsEditorRoot.simulator.runScript(script.text)
                }
            }

            Slider {
                id: speedSlider
                anchors.left: runButton.right
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
                anchors.left: speedSlider.right
                text: "Simulation speed: "+speedSlider.value
            }
        }

    }


}
