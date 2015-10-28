import QtQuick 2.0

import "qrc:/mobile/style"
import "../main-menu"

Rectangle {
    signal clickedSimulation(var simulation)
    clip: true
    color: "#333"

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
                            "qrc:/simulations/other/water",
                            "qrc:/simulations/diffusion/simple_diffusion",
                            "qrc:/simulations/other/indent"
                        ]
                        delegate: Item {
                            width: 100
                            height: 100
                            SimulationLoader {
                                id: loader
                                folder: modelData
                            }
                            Image {
                                anchors.fill: parent
                                source: loader.item ? loader.item.screenshotSource : ""
                                smooth: true
                                antialiasing: true
                                fillMode: Image.PreserveAspectCrop
                            }
                            MouseArea {
                                anchors.fill: parent
                                onClicked: clickedSimulation(loader.item)
                            }
                        }
                    }
                }
            }
        }
    }
}

