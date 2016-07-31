import QtQuick 2.5
import QtQuick.Layouts 1.2
import QtQuick.Controls 1.4
import Atomify 1.0
import "../plotting"
Rectangle {
    id: rectangleRoot
    property System system
    radius: 4
    color: Qt.rgba(1.0, 1.0, 1.0, 0.75)

    onSystemChanged: {
        if(system) {
            system.groups.onActiveChanged.connect(updateGroups)
            system.regions.onActiveChanged.connect(updateRegions)
            system.computes.onActiveChanged.connect(updateComputes)
        }
    }

    function getGlobalPosition(p, item) {
        console.log("Item: ", item)
        console.log("Parent: ", item.parent)
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

    function createComputeWindow(compute, point) {
        var component = Qt.createComponent("../plotting/ComputePlotter.qml");
        if (component.status == Component.Ready) {
            var computePlotter = component.createObject(rectangleRoot);
            computePlotter.x = point.x - computePlotter.width*0.5
            computePlotter.y = point.y - computePlotter.height*0.5
            computePlotter.compute = compute
            computePlotter.show()
        }

//        else
//            component.statusChanged.connect(finishComputeWindow);
    }

    Column {
        anchors.fill: parent
        spacing: 10
        GroupBox {
            width: parent.width
            title: "Simulation summary"

            Column {
                Text {
                    font.bold: true
                    text: "Camera position: ("+system.cameraPosition.x.toFixed(1)+", "+system.cameraPosition.y.toFixed(1)+", "+system.cameraPosition.z.toFixed(1)+")"
                }
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

                Column {
                    height: groupsRow.height + groupsList.height

                    Row {
                        id: groupsRow
                        spacing: 2
                        height: groupsLabel.height

                        Image {
                            id: collapseGroups
                            y: 3
                            source: "qrc:/images/expand.gif"
                            MouseArea {
                                anchors.fill: parent
                                onClicked: system.groups.active = !system.groups.active
                            }
                        }
                        Label {
                            id: groupsLabel
                            text: "Groups: "+system.groups.count
                            MouseArea {
                                anchors.fill: parent
                                onClicked: system.groups.active = !system.groups.active
                            }
                        }
                    }

                    ListView {
                        id: groupsList
                        anchors.top: groupsRow.bottom
                        x: groupsLabel.x
                        model: system ? system.groups.model : null
                        height: visible ? count*26 : 0
                        visible: false
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

                Column {
                    height: regionsRow.height + regionsList.height

                    Row {
                        id: regionsRow
                        spacing: 2
                        height: regionsLabel.height

                        Image {
                            id: collapseRegions
                            y: 3
                            source: "qrc:/images/expand.gif"
                            MouseArea {
                                anchors.fill: parent
                                onClicked: system.regions.active = !system.regions.active
                            }
                        }
                        Label {
                            id: regionsLabel
                            text: "Regions: "+system.regions.count
                            MouseArea {
                                anchors.fill: parent
                                onClicked: system.regions.active = !system.regions.active
                            }
                        }
                    }

                    ListView {
                        id: regionsList
                        anchors.top: regionsRow.bottom
                        x: regionsLabel.x
                        model: system ? system.regions.model : null
                        height: visible ? count*26 : 0
                        visible: false
                        delegate: Row {
                            spacing: 5
                            Image {
                                id: regionVisible
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
                                visible: regionsList.visible
                                text: model.modelData.identifier+": "+model.modelData.count+" atoms"
                                MouseArea {
                                    anchors.fill: parent
                                    hoverEnabled: true
                                    onClicked: model.modelData.visible = !model.modelData.visible
                                    onHoveredChanged: {
                                        model.modelData.hovered = containsMouse
                                    }
                                    cursorShape: Qt.PointingHandCursor
                                }
                            }

                        }
                    }
                }

                Column {
                    height: computesRow.height + computesList.height

                    Row {
                        id: computesRow
                        spacing: 2
                        height: computesLabel.height

                        Image {
                            id: collapseComputes
                            y: 3
                            source: "qrc:/images/expand.gif"
                            MouseArea {
                                anchors.fill: parent
                                onClicked: system.computes.active = !system.computes.active
                            }
                        }
                        Label {
                            id: computesLabel
                            text: "Computes: "+system.computes.count
                            MouseArea {
                                anchors.fill: parent
                                onClicked: system.computes.active = !system.computes.active
                            }
                        }
                    }

                    ListView {
                        id: computesList
                        anchors.top: computesRow.bottom
                        x: computesLabel.x
                        model: system ? system.computes.model : null
                        height: visible ? count*26 : 0
                        visible: false
                        delegate: Row {
                            visible: computesList.visible
                            Label {
                                id: computeTitleLabel
                                font.underline: true
                                color: "steelblue"
                                text: {
                                    model.modelData.identifier
                                }
                                MouseArea {
                                    anchors.fill: parent
                                    cursorShape: Qt.PointingHandCursor
                                    onClicked: {
                                        var point = Qt.point(mouseX, mouseY)
                                        point = getGlobalPosition(point, computeTitleLabel)
                                        createComputeWindow(model.modelData, point)
                                    }
                                }
                            }
                            Label {
                                text: {
                                    if(model.modelData.hasScalarData) {
                                        ": "+model.modelData.scalarValue.toFixed(3)
                                    } else {
                                        ""
                                    }
                                }
                            }
                        }
                    }
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
