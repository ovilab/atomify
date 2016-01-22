import QtQuick 2.0

import "qrc:/mobile/style"
import "../main-menu"

Rectangle {
    signal simulationClicked(var simulation)
    signal backClicked
    clip: true
    color: "#222"

    Flickable {
        anchors.fill: parent
        anchors.margins: Style.baseMargin
        contentWidth: width
        contentHeight: categories.height
        flickableDirection: Flickable.VerticalFlick
        Column {
            id: categories
            anchors{
                left: parent.left
                right: parent.right
            }
            spacing: Style.baseMargin

            Image {
                source: "qrc:/images/menu.png"
                width: Style.touchableSize
                height: width
                fillMode: Image.PreserveAspectFit
                smooth: true
                antialiasing: true

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        backClicked()
                    }
                }
            }

            Text {
                text: "Other"
                font.pixelSize: Style.font.button.size
                color: Style.font.color
            }
            Flickable {
                width: categories.width
                height: simulations.height
                contentHeight: height
                contentWidth: simulations.width
                flickableDirection: Flickable.HorizontalFlick
                Row {
                    id: simulations
                    spacing: Style.baseMargin
                    Repeater {
                        model: [
                            "qrc:/simulations/diffusion/simple_diffusion",
                            "qrc:/simulations/water/singlewater",
                            "qrc:/simulations/water/multiplewater",
                            "qrc:/simulations/other/indent",
                            "qrc:/simulations/silica/silica"
                        ]
                        delegate: Item {
                            width: Style.touchableSize * 3
                            height: width * 16.0 / 10.0
                            SimulationLoader {
                                id: loader
                                folder: modelData
                            }
                            Image {
                                anchors {
                                    left: parent.left
                                    right: parent.right
                                    top: parent.top
                                    bottom: title.top
                                }

                                source: loader.item ? loader.item.screenshotSource : ""
                                smooth: true
                                antialiasing: true
                                fillMode: Image.PreserveAspectCrop
                            }
                            Text {
                                id: title
                                anchors {
                                    bottom: parent.bottom
                                    horizontalCenter: parent.horizontalCenter
                                }
                                font.pixelSize: Style.font.size
                                color: Style.font.color
                                text: loader.item ? loader.item.name : "N/A"
                            }

                            MouseArea {
                                anchors.fill: parent
                                onClicked: simulationClicked(loader.item)
                            }
                        }
                    }
                }
            }
        }
    }
}

