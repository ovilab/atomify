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
            contentHeight: miniControlGrid.height
            GridLayout {
                id: miniControlGrid

                width: parent.width
                columns: 1
                columnSpacing: Style.spacing
                rowSpacing: Style.spacing
                property real itemSize: miniControlGrid.width / miniControlGrid.columns - miniControlGrid.columnSpacing * (miniControlGrid.columns - 1)

                Repeater {
                    id: repeater
                    model: simulation.controllers.length
                    Item {
                        width: 1
                        Layout.fillWidth: true
                        Layout.preferredHeight: 10/16*miniControlGrid.itemSize * Layout.rowSpan
                        Layout.columnSpan: miniControlLoader.item ? miniControlLoader.item.Layout.columnSpan : 1
                        Layout.rowSpan: miniControlLoader.item ? miniControlLoader.item.Layout.rowSpan : 1

                        Loader {
                            id: miniControlLoader
                            anchors.fill: parent
                            sourceComponent: simulation.controllers[index].miniControl

                            onLoaded: {
                                if(!(item && item.fixes && simulator)) {
                                    return
                                }

                                item.visualizer = visualizer
                                for(var j in item.fixes) {
                                    var fix = item.fixes[j]
                                    fix.parent = simulator
                                    console.log("Created " + fix + " on simulator " + simulator)
                                }
                            }
                        }

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                if(simulation.controllers[index].fullControl) {
                                    controlClicked(index)
                                }
                            }
                        }
                    }
                }
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

