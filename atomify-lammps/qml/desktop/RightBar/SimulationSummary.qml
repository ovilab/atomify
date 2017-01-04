import QtQuick 2.5
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.0
import Atomify 1.0
import "../../plotting"
import "../../visualization"

Flickable {
    id: root
    property var system: System {} /*{
        "groups": {
            "model": [{
                    "identifier": "test",
                    "count": 200
                },
                {
                    "identifier": "other",
                    "count": 200
                }]
        },
        "regions": {
            "model": [{
                    "identifier": "test",
                    "count": 200
                },
                {
                    "identifier": "other",
                    "count": 200
                }]
        },
        "fixes": {
            "model": [{
                    "identifier": "test",
                    "count": 200
                },
                {
                    "identifier": "other",
                    "count": 200
                }]
        },
        "computes": {
            "model": [{
                    "identifier": "test",
                    "count": 200
                },
                {
                    "identifier": "other",
                    "count": 200
                }]
        },
        "variables": {
            "model": [{
                    "identifier": "test",
                    "count": 200
                },
                {
                    "identifier": "other",
                    "count": 200
                }]
        }
    }*/

    property var visualizer: {
        "mode": "flymode"
    }

    flickableDirection: Flickable.VerticalFlick
    contentHeight: column.height + 16
    ScrollBar.vertical: ScrollBar {}
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
        id: column
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
            margins: 10
        }

        spacing: 10

        GroupBox {
            anchors {
                left: parent.left
                right: parent.right
            }

            title: "Camera"

            Column {
                Label {
                    text: "Position: ("+system.cameraPosition.x.toFixed(1)+", "+system.cameraPosition.y.toFixed(1)+", "+system.cameraPosition.z.toFixed(1)+")"
                }
                Label {
                    text: "Mode: " + (visualizer.mode === "flymode" ? "Flymode" : "Trackball")
                }
            }
        }

        GroupBox {
            anchors {
                left: parent.left
                right: parent.right
            }

            title: "System"

            Column {
                Label {
                    text: "Size: ("+system.size.x.toFixed(1)+", "+system.size.y.toFixed(1)+", "+system.size.z.toFixed(1)+")"
                }
                Label {
                    text: "Volume: "+system.volume.toFixed(2)
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
            }
        }

        GroupBox {
            anchors {
                left: parent.left
                right: parent.right
            }

            title: "Time"

            Column {
                Label {
                    text: "Current timestep: "+system.currentTimestep
                }
                Label {
                    text: "Time: "+system.simulationTime.toFixed(2)
                }
            }
        }

        GroupBox {
            anchors {
                left: parent.left
                right: parent.right
            }

            title: "Groups"

            Column {
                id: groupsColumn

                Repeater {
                    id: groupsList
                    model: system ? system.groups.model : undefined
                    height: visible ? count*26 : 0
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
        }

        GroupBox {
            anchors {
                left: parent.left
                right: parent.right
            }

            title: "Regions"

            RegionsColumn {
                id: regionsColumn
                system: root.system
            }
        }


        GroupBox {
            anchors {
                left: parent.left
                right: parent.right
            }

            title: "Computes"

            ComputesColumn {
                id: computesColumn
                system: root.system
            }
        }

        GroupBox {
            anchors {
                left: parent.left
                right: parent.right
            }

            title: "Variables"

            VariablesColumn {
                id: variablesColumn
                system: root.system
            }
        }

        GroupBox {
            anchors {
                left: parent.left
                right: parent.right
            }

            title: "Fixes"

            FixesColumn {
                id: fixesColumn
                system: root.system
            }
        }
        //        GroupBox {
        //            id: keyboardShortcuts
        //            width: parent.width
        //            title: "Keyboard shortcuts"

        //            Column {
        //                id: shortcutRoot
        //                width: parent.width
        //                property int labelWidth: 115
        //                property string controlName: {
        //                    if(Qt.platform.os === "osx") {
        //                        return "⌘"
        //                    }
        //                    return "Ctrl+"
        //                }

        //                RowLayout {
        //                    width: parent.width
        //                    Label {
        //                        Layout.minimumWidth: shortcutRoot.labelWidth
        //                        Layout.maximumWidth: shortcutRoot.labelWidth
        //                        text: "Run script "
        //                    }
        //                    Label {
        //                        text: ": "+shortcutRoot.controlName+"R"
        //                    }
        //                }

        //                RowLayout {
        //                    width: parent.width
        //                    Label {
        //                        Layout.minimumWidth: shortcutRoot.labelWidth
        //                        Layout.maximumWidth: shortcutRoot.labelWidth
        //                        text: "New script "
        //                    }
        //                    Label {
        //                        text: ": "+shortcutRoot.controlName+"N"
        //                    }
        //                }

        //                RowLayout {
        //                    width: parent.width
        //                    Label {
        //                        Layout.minimumWidth: shortcutRoot.labelWidth
        //                        Layout.maximumWidth: shortcutRoot.labelWidth
        //                        text: "Save script "
        //                    }
        //                    Label {
        //                        text: ": "+shortcutRoot.controlName+"S"
        //                    }
        //                }

        //                RowLayout {
        //                    width: parent.width
        //                    Label {
        //                        Layout.minimumWidth: shortcutRoot.labelWidth
        //                        Layout.maximumWidth: shortcutRoot.labelWidth
        //                        text: "Open script"
        //                    }
        //                    Label {
        //                        text: ": "+shortcutRoot.controlName+"O"
        //                    }
        //                }

        //                RowLayout {
        //                    width: parent.width
        //                    Label {
        //                        Layout.minimumWidth: shortcutRoot.labelWidth
        //                        Layout.maximumWidth: shortcutRoot.labelWidth
        //                        text: "Toggle pause"
        //                    }
        //                    Label {
        //                        text: ": "+shortcutRoot.controlName+"P / Space"
        //                    }
        //                }

        //                RowLayout {
        //                    width: parent.width
        //                    Label {
        //                        Layout.minimumWidth: shortcutRoot.labelWidth
        //                        Layout.maximumWidth: shortcutRoot.labelWidth
        //                        text: "Toggle focus mode "
        //                    }
        //                    Label {
        //                        text: ": Tab"
        //                    }
        //                }
        //            }
        //        }
    }
}
