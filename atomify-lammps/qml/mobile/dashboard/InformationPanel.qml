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
            contentHeight: textColumn.height
            Column {
                id: textColumn
                width: parent.width
                Text {
                    width: parent.width
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    font.pixelSize: Style.font.heading.size
                    color: Style.font.heading.color
                    text: simulation ?  simulation.name : ""
                }
                Text {
                    width: parent.width
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    font.pixelSize: Style.font.size
                    color: Style.font.color
                    textFormat: Text.RichText
                    text: simulation ? simulation.description : ""
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

