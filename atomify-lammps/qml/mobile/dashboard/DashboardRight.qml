import QtQuick 2.0
import QtQuick.Layouts 1.1

import Atomify 1.0

import "qrc:/core"
import "qrc:/mobile/style"
import "qrc:/visualization"

Item {
    id: dashboardRoot

    signal continueClicked
    signal controlClicked(var index)

    property AtomifySimulator simulator: null
    property Simulation simulation: null
    property bool revealed: false
    property AtomifyVisualizer visualizer

    state: revealed ? "" : "hidden"

    width: parent.width
    height: parent.height

    anchors {
        rightMargin: 0
        right: parent.right
        top: parent.top
        bottom: parent.bottom
    }

    onSimulationChanged: {
        var menuIndex = 0
        for(var i in simulation.controllers) {
            var controller = simulation.controllers[i]
            var miniControl = simulation.controllers[i].miniControl
            if(!miniControl) {
                continue
            }
            // TODO control should be removed again when changing simulation
            miniControl.parent = gridLayout
            miniControl.itemSize = Qt.binding(function() {return gridLayout.itemSize})
            var fullControl = simulation.controllers[i].fullControl
            if(!fullControl) {
                continue
            }
            miniControl.menuIndex = menuIndex
            miniControl.clicked.connect(controlClicked)
            menuIndex += 1
        }
    }

    Rectangle {
        id: contents
        anchors {
            right: parent.right
            top: parent.top
            bottom: parent.bottom
        }

        color: "#333"
        width: Math.min(parent.width, parent.height) * 0.85

        Flickable {
            anchors {
                fill: parent
                margins: Style.baseMargin
            }
            contentHeight: gridLayout.height
            GridLayout {
                id: gridLayout

                width: parent.width
                columns: 2
                columnSpacing: 0
                rowSpacing: 0
                property real itemSize: gridLayout.width / gridLayout.columns - gridLayout.columnSpacing * (gridLayout.columns - 1)

//                Repeater {
//                    id: repeater
//                    model: simulation.controllers.length
//                    Loader {

//                        Layout.fillWidth: true
//                        //                        Layout.preferredWidth: itemSize
//                        Layout.preferredHeight: itemSize * Layout.rowSpan
//                        Layout.columnSpan: item ? item.Layout.columnSpan ? item.Layout.columnSpan : 1 : 1
//                        Layout.rowSpan: item ? item.Layout.rowSpan ? item.Layout.rowSpan : 1 : 1

//                        sourceComponent: simulation.controllers[index]

//                        onLoaded: {
//                            if(!(item && item.fixes && simulator)) {
//                                return
//                            }

//                            item.visualizer = visualizer
//                            for(var j in item.fixes) {
//                                var fix = item.fixes[j]
//                                fix.parent = simulator
//                                console.log("Created " + fix + " on simulator " + simulator)
//                            }
//                        }

//                        Connections {
//                            target: item
//                            onClicked: {
//                                controlClicked(item.controlComponents)
//                            }
//                        }
//                    }
//                }
            }
        }
    }

    MouseArea {
        anchors {
            right: contents.left
            left: parent.left
            top: parent.top
            bottom: parent.bottom
        }
        propagateComposedEvents: true
        onClicked: {
            if(revealed) {
                revealed = false
            } else {
                mouse.accepted = false
            }
        }
    }

    states: [
        State {
            name: "hidden"
            PropertyChanges {
                target: dashboardRoot
                anchors.rightMargin: -dashboardRoot.width
            }
        }
    ]

    transitions: [
        Transition {
            NumberAnimation {
                target: dashboardRoot
                property: "anchors.rightMargin"
                duration: 360
                easing.type: Easing.InOutQuad
            }
        }
    ]
}

