import QtQuick 2.5
import QtQuick.Layouts 1.2
import QtQuick.Controls 1.4
import Atomify 1.0

Rectangle {
    property System system
    radius: 4
    color: Qt.rgba(1.0, 1.0, 1.0, 0.75)

    Column {
        anchors.fill: parent
        spacing: 10
        GroupBox {
            width: parent.width
            title: "Simulation summary"

            ColumnLayout {
                Text {
                    font.bold: true
                    text: "System size: ("+system.size.x.toFixed(1)+", "+system.size.y.toFixed(1)+", "+system.size.z.toFixed(1)+")"
                }
                Text {
                    font.bold: true
                    text: "System volume: "+system.volume
                }
                Text {
                    font.bold: true
                    text: "Number of atoms: "+system.numberOfAtoms
                }
                Text {
                    font.bold: true
                    text: "Average density: "+(system.numberOfAtoms/system.volume).toFixed(4)
                }
                Text {
                    font.bold: true
                    text: "Number of atom types: "+system.numberOfAtomTypes
                }

                Text {
                    font.bold: true
                    text: "Timesteps: "+system.timesteps
                }

                Text {
                    font.bold: true
                    text: "Time: "+system.simulationTime.toFixed(2)
                }
            }
        }

        GroupBox {
            width: parent.width
            title: "Groups"

            ListView {
                model: system ? system.groups.model : null
                height: count*26
                delegate: Label {
                    text: model.modelData.name+": "+model.modelData.count+" atoms"
                }
            }
        }

        GroupBox {
            width: parent.width
            title: "Regions"
            visible: system.regions.count>0

            ListView {
                model: system ? system.regions.model : null
                height: count*26
                delegate: Label {
                    text: model.modelData.name+": "+model.modelData.count+" atoms"
                }
            }
        }

        GroupBox {
            width: parent.width
            title: "Keyboard shortcuts"

            ColumnLayout {
                id: shortcutRoot
                property int labelWidth: 115
                property string controlName: {
                    if(Qt.platform.os === "osx") {
                        return "⌘"
                    }
                    return "Ctrl"
                }

                RowLayout {
                    Layout.fillWidth: true
                    Label {
                        Layout.minimumWidth: shortcutRoot.labelWidth
                        Layout.maximumWidth: shortcutRoot.labelWidth
                        text: "Run script "
                    }
                    Label {
                        text: ": "+shortcutRoot.controlName+"R"
                    }
                }

                RowLayout {
                    Layout.fillWidth: true
                    Label {
                        Layout.minimumWidth: shortcutRoot.labelWidth
                        Layout.maximumWidth: shortcutRoot.labelWidth
                        text: "Show editor tab "
                    }
                    Label {
                        text: ": "+shortcutRoot.controlName+"1"
                    }
                }

                RowLayout {
                    Layout.fillWidth: true
                    Label {
                        Layout.minimumWidth: shortcutRoot.labelWidth
                        Layout.maximumWidth: shortcutRoot.labelWidth
                        text: "Show analysis tab "
                    }
                    Label {
                        text: ": "+shortcutRoot.controlName+"2"
                    }
                }

                RowLayout {
                    Layout.fillWidth: true
                    Label {
                        Layout.minimumWidth: shortcutRoot.labelWidth
                        Layout.maximumWidth: shortcutRoot.labelWidth
                        text: "Toggle focus mode "
                    }
                    Label {
                        text: ": Tab"
                    }
                }
            }
        }
    }
}
