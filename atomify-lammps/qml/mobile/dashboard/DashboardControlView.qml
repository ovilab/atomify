import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0

import Atomify 1.0

import "qrc:/core"
import "qrc:/mobile/style"
import "qrc:/visualization"

Rectangle {
    id: root

    signal backClicked
    property AtomifyVisualizer visualizer
    property AtomifySimulator simulator
    property Simulation simulation
    property int currentIndex
    property bool active: false
    onActiveChanged: {
        simulation.controllers[currentIndex].active = active
    }

    width: parent.width
    height: parent.height
    color: "#222"
    clip: true

    function gotoIndex(index) {
        flickable.contentX = flickable.width * index
    }

    Flickable {
        anchors {
            fill: parent
        }
        contentWidth: width
        contentHeight: dummy.height

        Item {
            id: dummy
            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
                bottom: controllerColumn.bottom
            }
        }

        Image {
            id: backButton
            anchors {
                left: parent.left
                top: parent.top
                margins: Style.baseMargin
            }

            width: Style.touchableSize
            height: width

            source: "qrc:/images/back.png"
            smooth: true
            antialiasing: true

            MouseArea {
                anchors.fill: parent
                onClicked: backClicked()
            }
        }

        Text {
            anchors {
                top: backButton.top
                horizontalCenter: parent.horizontalCenter
            }
            text: "Dashboard"
            font.pixelSize: Style.font.heading.size
            color: Style.font.heading.color
        }

        Grid {
            id: controlList
            anchors {
                top: backButton.bottom
                left: parent.left
                margins: Style.baseMargin
            }
            rows: root.width > root.height ? 0 : 1
            columns: root.width > root.height ? 1 : 0
            Repeater {
                model: simulation ? simulation.controllers.length : undefined
                Rectangle {
                    width: Style.touchableSize * 3.5
                    height: Style.touchableSize * 1.5
                    color: currentIndex === index ? "white" : "transparent"
                    border {
                        color: "white"
                        width: 2.0
                    }
                    Text {
                        text: simulation.controllers[index].name
                        color: currentIndex === index ? "black" : Style.font.color
                        anchors.centerIn: parent
                        font.pixelSize: Style.font.size
                    }
                    MouseArea {
                        anchors.fill: parent
                        onClicked: currentIndex = index
                    }
                }
            }
        }

        Column {
            id: controllerColumn
            anchors {
                left: root.width > root.height ? controlList.right : parent.left
                right: parent.right
                top: root.width > root.height ? backButton.bottom : controlList.bottom
                margins: Style.baseMargin
            }

            Loader {
                id: controllerLoader
                anchors {
                    left: parent.left
                    right: parent.right
                }
                sourceComponent: simulation.controllers[currentIndex].fullControl
            }
        }
    }
}

