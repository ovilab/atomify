import QtQuick 2.5

import "qrc:/core"
import "qrc:/mobile/style"

Rectangle {
    id: simulationRoot
    property Simulation simulation: null
    signal playClicked(var simulation)
    signal backClicked
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

                Image {
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

                Image {
                    anchors.centerIn: parent
                    width: Style.touchableSize * 2
                    height: width

                    source: "qrc:/images/play.png"
                    smooth: true
                    antialiasing: true

                    MouseArea {
                        anchors.fill: parent
                        onClicked: playClicked(simulation)
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

