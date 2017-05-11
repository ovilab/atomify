import QtQuick 2.5
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.1
import Atomify 1.0
import "../items"
import "../../plotting"
import "../../visualization"

Flickable {
    id: root
    property var simulator
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
                    text: {
                        var state = ""
                        if(simulator.states.paused.active) {
                            state = "Paused"
                        } else if(simulator.states.finished.active) {
                            state = "Finished"
                        } else {
                            state = system.state
                        }

                        return "State: "+state
                    }

                    visible: system.state!==""
                }

                Label {
                    text: "Triclinic"
                    visible: system.triclinic
                }
                Label {
                    text: "Units: <a href=\"units\">"+system.units.name+"</a>"
                    onLinkActivated: {
                        Qt.openUrlExternally("http://lammps.sandia.gov/doc/units.html")
                    }
                }
                ToolTipLabel {
                    toolTipText: "The current simulations is "+system.units.dimensions+"-dimensional."
                    text: "Dimensions: "+system.units.dimensions
                }
                ToolTipLabel {
                    property string explainationP: (system.boundaryStyle.search("p")>-1) ? "p is periodic. " : ""
                    property string explainationF: (system.boundaryStyle.search("f")>-1) ? "f is non-periodic and fixed. " : ""
                    property string explainationS: (system.boundaryStyle.search("s")>-1) ? "s is non-periodic and shrink-wrapped. " : ""
                    property string explainationM: (system.boundaryStyle.search("m")>-1) ? "m is non-periodic and shrink-wrapped with a minimum value. " : ""
                    toolTipText: explainationP+explainationF+explainationS+explainationM
                    text: "Boundary: "+system.boundaryStyle
                }
                Label {
                    text: "Timestep: "+system.dt+(system.units.time==="" ? "" : " ["+system.units.time+"]")
                }
                Label {
                    text: "Number of atoms: "+system.numberOfAtoms
                }
                Label {
                    text: "Number of bonds: "+system.atoms.numberOfBonds
                    visible: system.atoms.numberOfBonds>0
                }
                Label {
                    text: "Size: ("+system.size.x.toFixed(1)+", "+system.size.y.toFixed(1)+", "+system.size.z.toFixed(1)+")"+(system.units.length==="" ? "" : " ["+system.units.length+"]")
                }
                Label {
                    text: "Center: ("+system.center.x.toFixed(1)+", "+system.center.y.toFixed(1)+", "+system.center.z.toFixed(1)+")"+(system.units.length==="" ? "" : " ["+system.units.length+"]")
                }
                Label {
                    textFormat: Qt.RichText
                    text: "Volume: "+system.volume.toPrecision(2)+(system.units.volume==="" ? "" : " ["+system.units.volume+"]")
                }
                Label {
                    textFormat: Qt.RichText
                    text: "Average density: "+(system.numberOfAtoms/(system.volume===0 ? 1 : system.volume) ).toPrecision(2)+(system.units.density==="" ? "" : " ["+system.units.density+"]")
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
                anchors {
                    left: parent.left
                    right: parent.right
                }
                Label {
                    text: "Current timestep: "+system.currentTimestep
                }
                Label {
                    text: "Time: "+system.simulationTime.toFixed(2)+ (system.units.time === "" ? "" : " ["+system.units.time+"]")
                }
                ToolTipLabel {
                    toolTipText: "1x targets 60 frames per second."
                    text: "Target speed: "+speedSlider.value.toFixed(0)+"x"
                }
                Slider {
                    id: speedSlider
                    anchors {
                        left: parent.left
                        right: parent.right
                    }
                    from: 1
                    to: 100
                    stepSize: 1
                    snapMode: Slider.SnapAlways
                    value: simulator ? simulator.simulationSpeed : 1
                    onValueChanged: if(simulator !== undefined) { simulator.simulationSpeed = value }
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
                Label {
                    text: "No groups"
                    visible: groupsList.count==0
                }

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

        GroupBox {
            anchors {
                left: parent.left
                right: parent.right
            }

            title: "Performance"

            Column {
                Label {
                    text: "FPS: "+ root.visualizer.fps
                }
                Label {
                    text: "Memory usage LAMMPS: "+ (system.performance.memoryLAMMPS / 1024 / 1024).toFixed(0) +" MB"
                }
                Label {
                    text: "Memory usage Atomify: "+ (system.performance.memoryAtomify / 1024 / 1024).toFixed(0) +" MB"
                }
            }
        }
    }
}
