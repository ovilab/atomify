import QtQuick 2.0

import "qrc:/core"
import "qrc:/mobile/style"

Rectangle {
    id: simulationRoot
    property Simulation simulation: null
    signal loadSimulation(var simulation)
    width: 100
    height: 100
    color: "#333"

    Flickable {
        anchors.fill: parent
        contentWidth: width
        contentHeight: contents.height
        flickableDirection: Flickable.VerticalFlick

        Column {
            id: contents
            anchors{
                left: parent.left
                right: parent.right
            }
            spacing: Style.baseMargin
            Image {
                anchors {
                    left: parent.left
                    right: parent.right
                }
                height: simulationRoot.height * 0.3

                source: simulation ? simulation.screenshotSource : ""
                smooth: true
                antialiasing: true
                fillMode: Image.PreserveAspectCrop

                Rectangle {
                    anchors.centerIn: parent
                    width: Style.touchableSize * 2
                    height: width

                    color: Qt.rgba(1.0, 1.0, 1.0, 0.5)

                    MouseArea {
                        anchors.fill: parent
                        onClicked: loadSimulation(simulation)
                    }
                }
            }
            Column {
                anchors {
                    left: parent.left
                    right: parent.right
                    margins: Style.baseMargin
                }
                spacing: Style.baseMargin

                Text {
                    font.pixelSize: Style.font.heading.size
                    color: Style.font.heading.color
                    text: simulation ? simulation.name : "N/A"
                }

                Text {
                    anchors {
                        left: parent.left
                        right: parent.right
                    }
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    color: Style.font.color
                    font.pixelSize: Style.font.size
                    text: simulation ? simulation.description : "N/A"
                }
            }
        }
    }
}

