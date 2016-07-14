import QtQuick 2.0

import "qrc:/core"
import "qrc:/mobile/style"

Item {
    id: mainMenuRoot

    signal continueClicked
    signal simulationsClicked
    signal simulationClicked(var simulation)

    property bool revealed: false
    property Simulation simulation: null

    state: revealed ? "" : "hidden"

    width: parent.width
    height: parent.height

    anchors {
        leftMargin: 0
        left: parent.left
        top: parent.top
        bottom: parent.bottom
    }

    Rectangle {
        id: contents
        anchors {
            left: parent.left
            top: parent.top
            bottom: parent.bottom
        }

        color: "#333"
        width: Math.min(parent.width, parent.height) * 0.85

        MouseArea {
            anchors.fill: parent
            onWheel: {
            }
        }

        Flickable {
            anchors.fill: parent
            contentHeight: contentColumn.height
            flickableDirection: Flickable.VerticalFlick
            Column {
                id: contentColumn
                anchors {
                    margins: 0
                }
                width: parent.width
                Image {
                    width: parent.width
                    height: 9 / 16 * width
                    source: simulation ? simulation.screenshotSource : "qrc:/images/systems.png"
                    fillMode: Image.PreserveAspectCrop

                    Text {
                        anchors {
                            left: simulationNameText.left
                            bottom: simulationNameText.top
                        }

                        text: "Currently simulating"
                        color: Style.font.color
                        font.pixelSize: Style.font.size * 0.8
                        font.weight: Font.Light
                        style: Text.Outline
                        styleColor: Style.font.heading.color
                    }

                    Text {
                        id: simulationNameText
                        anchors {
                            left: parent.left
                            bottom: parent.bottom
                            leftMargin: Style.baseMargin
                            bottomMargin: Style.baseMargin * 0.5
                        }

                        color: Style.font.color
                        font.pixelSize: Style.font.size
                        font.weight: Font.Light
                        text: simulation ? simulation.name : "N/A"
                        style: Text.Outline
                        styleColor: Style.font.heading.color
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            simulationClicked(simulation)
                        }
                    }
                }
                Column {
                    anchors {
                        left: parent.left
                        right: parent.right
                        margins: Style.baseMargin
                    }
                    MainMenuLeftButton {
                        text: qsTr("Return to simulation")
                        onClicked: {
                            revealed = false
                            continueClicked()
                        }
                    }
                    MainMenuLeftButton {
                        text: qsTr("Select simulation")
                        onClicked: {
                            revealed = false
                            simulationsClicked()
                        }
                    }
                }
            }
        }

    }

    MouseArea {
        anchors {
            left: contents.right
            right: parent.right
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
                target: mainMenuRoot
                anchors.leftMargin: -mainMenuRoot.width
            }
        }
    ]

    transitions: [
        Transition {
            NumberAnimation {
                target: mainMenuRoot
                property: "anchors.leftMargin"
                duration: 360
                easing.type: Easing.InOutQuad
            }
        }
    ]
}

