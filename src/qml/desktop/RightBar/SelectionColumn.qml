import QtQuick 2.5
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.2
import QtQuick.Controls 1.4 as QQC1
import Atomify 1.0
import "../../plotting"
import "SimulatorControlItems"

Column {
    id: root
    property var system

    Label {
        text: "No selection"
        visible: list.count==0
    }

    Repeater {
        id: list
        model: system ? system.selection : null
        delegate: Column {
            spacing: 5
            Row {
                Label {
                    text: {
                        "Atom "+model.modelData.id
                    }
                    font.bold: true
                }
            }
            Row {
                Label {
                    text: {
                        "Type: "+model.modelData.type
                    }
                }
            }
            Row {
                Label {
                    text: {
                        var pos = model.modelData.position
                        "Position: ("+pos.x.toPrecision(4)+", "+pos.y.toPrecision(4)+", "+pos.z.toPrecision(4)+")"
                    }
                }
            }
            Row {
                Label {
                    text: {
                        var vel = model.modelData.velocity
                        "Velocity: ("+vel.x.toPrecision(4)+", "+vel.y.toPrecision(4)+", "+vel.z.toPrecision(4)+")"
                    }
                }
            }
            Row {
                Label {
                    text: {
                        var force = model.modelData.velocity
                        "Force: ("+force.x.toPrecision(4)+", "+force.y.toPrecision(4)+", "+force.z.toPrecision(4)+")"
                    }
                }
            }

            Item {
                anchors.left: parent.left
                anchors.right: parent.right
                height: 5
            }

            Rectangle {
                anchors.left: parent.left
                anchors.right: parent.right
                height: 2
                color: "white"
            }

            Item {
                anchors.left: parent.left
                anchors.right: parent.right
                height: 10
            }
        }
    }
}

