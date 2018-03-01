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
    visible: list.count > 0

    Repeater {
        id: list
        model: system ? system.selection : null
        delegate: GroupBox {
            anchors {
                left: parent.left
                right: parent.right
            }

            hoverEnabled: true
            onHoveredChanged: model.modelData.hovered = hovered

            background: Rectangle {
                y: parent.topPadding - parent.padding
                width: parent.width
                height: parent.height - parent.topPadding + parent.padding
                color: "transparent"
                border.color: (model.modelData.hovered || model.modelData.marked) ? "#fff" : "#6e6e6e"
                border.width: model.modelData.marked ? 2 : 1
                radius: 2
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        // TODO(anders.hafreager) This feature is nice, but requires some more
                        // work for it to work propely.
                        // model.modelData.marked = !model.modelData.marked
                    }
                }
            }

            Column {
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
                            var force = model.modelData.force
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
}

