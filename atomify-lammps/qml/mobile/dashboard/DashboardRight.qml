import QtQuick 2.0
import QtQuick.Layouts 1.1

import "qrc:/mobile/style"
import "qrc:/core"

Item {
    id: dashboardRoot

    signal continueClicked
    signal simulationsClicked

    property bool revealed: false
    property Simulation simulation: null

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
//        for(var i in simulation.controllers) {
//            var component = Qt.createComponent(simulation.controllers[i])
//            if (component.status !== Component.Ready) {
//                // Error Handling
//                console.log("Error loading component:", component.errorString());
//            }
//            var object = component.createObject(gridLayout, {width: gridLayout.itemSize, height: gridLayout.itemSize})
//        }
//        console.log(simulation.controllers.count)
//        repeater.model = simulation.controllers.count
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

                Repeater {
                    id: repeater
                    model: simulation.controllers.length
                    Loader {
                        property real itemSize: gridLayout.width / gridLayout.columns - gridLayout.columnSpacing * (gridLayout.columns - 1)

                        Layout.fillWidth: true
//                        Layout.preferredWidth: itemSize
                        Layout.preferredHeight: itemSize * Layout.rowSpan
                        Layout.columnSpan: item ? item.Layout.columnSpan ? item.Layout.columnSpan : 1 : 1
                        Layout.rowSpan: item ? item.Layout.rowSpan ? item.Layout.rowSpan : 1 : 1

                        sourceComponent: simulation.controllers[index]
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

