import QtQuick 2.0

import "qrc:/mobile/style"
import "../main-menu"

Rectangle {
    id: simulationsViewRoot

    signal simulationClicked(var simulation)
    signal backClicked
    clip: true
    color: "#222"

    Flickable {
        anchors.fill: parent
        anchors.margins: Style.baseMargin
//        anchors.margins: 0.0
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
                source: "qrc:/images/back.png"
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
            Repeater {
                model: [
                    {
                        name: "Water",
                        simulations: [
                            "qrc:/simulations/water/singlewater",
                            "qrc:/simulations/water/multiplewater",
                        ]
                    },
                    {
                        name: "Diffusion",
                        simulations: [
                            "qrc:/simulations/diffusion/simple_diffusion",
                        ]
                    },
                    {
                        name: "Porous media",
                        simulations: [
                            "qrc:/simulations/silica/nanoporous",
                            "qrc:/simulations/silica/silica",
                        ]
                    },
                    {
                        name: "Other",
                        simulations: [
                            "qrc:/simulations/other/indent",
                        ]
                    }
                ]
                Column {
                    spacing: Style.baseMargin
                    Text {
                        text: modelData.name
                        font.pixelSize: Style.font.button.size
                        color: Style.font.heading.color
                    }
                    Flickable {
                        width: categories.width
                        height: simulationsRow.height
                        contentHeight: height
                        contentWidth: simulationsRow.width
                        flickableDirection: Flickable.HorizontalFlick
                        Row {
                            id: simulationsRow
                            spacing: Style.baseMargin * 0.5
//                            spacing: 0.0
                            Repeater {
                                model: modelData.simulations
                                Item {
                                    width: simulationColumn.width
                                    height: simulationColumn.height
                                    SimulationLoader {
                                        id: loader
                                        folder: modelData
                                    }
                                    Column {
                                        id: simulationColumn
                                        Image {
                                            id: simulationImage
                                            property int simulationsCount: simulationsViewRoot.width / (Style.touchableSize * 2.5 + Style.baseMargin)
                                            onSimulationsCountChanged: {
                                                console.log(simulationsCount)
                                            }

                                            width: (simulationsViewRoot.width - Style.baseMargin) / (simulationsCount + 0.3) - simulationsRow.spacing
                                            height: width * 16.0 / 10.0

                                            source: loader.item ? loader.item.screenshotSource : ""
                                            smooth: true
                                            antialiasing: true
                                            fillMode: Image.PreserveAspectCrop
                                        }
                                        Text {
                                            id: title
                                            font.pixelSize: Style.font.size
                                            color: Style.font.color
                                            text: loader.item ? loader.item.name : "N/A"
                                            width: simulationImage.width
//                                            elide: Text.ElideRight
                                            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                                            horizontalAlignment: Text.AlignHCenter

                                        }
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
    }
}

