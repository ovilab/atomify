import QtQuick 2.5
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.0
import Atomify 1.0
import "../../plotting"

Flickable {
    id: rectangleRoot
    property System system

    function getGlobalPosition(p, item) {
        var globalX = p.x
        var globalY = p.y
        while(item.parent != undefined) {
            globalX = globalX + item.x
            globalY = globalY + item.y
            item = item.parent
        }
        return Qt.point(globalX, globalY)
    }

    function updateGroups() {
        groupsList.visible = system.groups.active
        if(system.groups.active) {
            collapseGroups.source = "qrc:/images/collapse.gif"
        } else {
            collapseGroups.source = "qrc:/images/expand.gif"
        }
    }

    function updateRegions() {
        regionsList.visible = system.regions.active
        if(system.regions.active) {
            collapseRegions.source = "qrc:/images/collapse.gif"
        } else {
            collapseRegions.source = "qrc:/images/expand.gif"
        }
    }

    function updateComputes() {
        computesList.visible = system.computes.active
        if(system.computes.active) {
            collapseComputes.source = "qrc:/images/collapse.gif"
        } else {
            collapseComputes.source = "qrc:/images/expand.gif"
        }
    }

    Column {
        anchors {
            fill: parent
            margins: 10
        }

        spacing: 10
        GroupBox {
            width: parent.width
            title: "Simulation summary"

            Column {
                Label {
                    text: "Camera position: ("+system.cameraPosition.x.toFixed(1)+", "+system.cameraPosition.y.toFixed(1)+", "+system.cameraPosition.z.toFixed(1)+")"
                }
                Label {
                    text: "System size: ("+system.size.x.toFixed(1)+", "+system.size.y.toFixed(1)+", "+system.size.z.toFixed(1)+")"
                }
                Label {
                    text: "System volume: "+system.volume.toFixed(2)
                }
                Label {
                    text: "Units: "+system.units.name
                }
                Label {
                    text: "Number of atoms: "+system.numberOfAtoms
                }
                Label {
                    text: "Average density: "+(system.numberOfAtoms/system.volume).toFixed(4)
                }
                Label {
                    text: "Number of atom types: "+system.numberOfAtomTypes
                }

                Label {
                    text: "Timesteps: "+system.timesteps
                }

                Label {
                    text: "Time: "+system.simulationTime.toFixed(2)
                }

                Column {
                    id: groupsColumn
                    property bool expanded
                    height: groupsRow.height + groupsList.height

                    Row {
                        id: groupsRow
                        spacing: 2
                        height: groupsLabel.height

                        Image {
                            id: collapseGroups
                            y: 3
                            source: groupsColumn.expanded ? "qrc:/images/collapse.gif" : "qrc:/images/expand.gif"
                            MouseArea {
                                anchors.fill: parent
                                onClicked: groupsColumn.expanded = !groupsColumn.expanded
                            }
                        }
                        Label {
                            id: groupsLabel
                            text: "Groups: "+system.groups.count
                            MouseArea {
                                anchors.fill: parent
                                onClicked: groupsColumn.expanded = !groupsColumn.expanded
                            }
                        }
                    }

                    ListView {
                        id: groupsList
                        anchors.top: groupsRow.bottom
                        x: groupsLabel.x
                        model: system ? system.groups.model : null
                        height: visible ? count*26 : 0
                        visible: groupsColumn.expanded
                        delegate: Row {
                            spacing: 5
                            Image {
                                id: groupVisible
                                width: 15
                                height: 15
                                y: 1
                                source: model.modelData.visible ? "qrc:/images/eye-on.png" : "qrc:/images/eye-off.png"
                                MouseArea {
                                    anchors.fill: parent
                                    hoverEnabled: true
                                    onHoveredChanged: {
                                        model.modelData.hovered = containsMouse
                                    }
                                    onClicked: model.modelData.visible = !model.modelData.visible
                                    cursorShape: Qt.PointingHandCursor
                                }
                            }
                            Label {
                                id: groupLabel
                                visible: groupsList.visible
                                text: model.modelData.identifier+": "+model.modelData.count+" atoms"
                                MouseArea {
                                    anchors.fill: parent
                                    hoverEnabled: true
                                    onClicked: model.modelData.visible = !model.modelData.visible
                                    onHoveredChanged: {
                                        model.modelData.hovered = containsMouse
                                    }
                                }
                            }
                        }
                    }
                }

                RegionsColumn {
                    id: regionsColumn
                }

                ComputesColumn {
                    id: computesColumn
                }

                VariablesColumn {
                    id: variablesColumn
                }

                FixesColumn {
                    id: fixesColumn
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
                    return "Ctrl+"
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
                        text: "New script "
                    }
                    Label {
                        text: ": "+shortcutRoot.controlName+"N"
                    }
                }

                RowLayout {
                    Layout.fillWidth: true
                    Label {
                        Layout.minimumWidth: shortcutRoot.labelWidth
                        Layout.maximumWidth: shortcutRoot.labelWidth
                        text: "Save script "
                    }
                    Label {
                        text: ": "+shortcutRoot.controlName+"S"
                    }
                }

                RowLayout {
                    Layout.fillWidth: true
                    Label {
                        Layout.minimumWidth: shortcutRoot.labelWidth
                        Layout.maximumWidth: shortcutRoot.labelWidth
                        text: "Open script"
                    }
                    Label {
                        text: ": "+shortcutRoot.controlName+"O"
                    }
                }

                RowLayout {
                    Layout.fillWidth: true
                    Label {
                        Layout.minimumWidth: shortcutRoot.labelWidth
                        Layout.maximumWidth: shortcutRoot.labelWidth
                        text: "Toggle pause"
                    }
                    Label {
                        text: ": "+shortcutRoot.controlName+"P / Space"
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
