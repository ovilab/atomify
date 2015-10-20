import QtQuick 2.5
import Qt.labs.settings 1.0
import "qrc:/mobile/style"
import "qrc:/core"

Item {
    id: buttonRoot
    property Simulation simulation: simulationLoader.item
//    property string simulationSource
    property alias folder: simulationLoader.folder
    signal loadSimulation(var fileName)
    signal readMore(var text)

    SimulationLoader {
        id: simulationLoader
    }

    Heading {
        id: nameText
        anchors {
            top: parent.top
            horizontalCenter: parent.horizontalCenter
            topMargin: Style.baseMargin
        }
        text: simulation ? simulation.name : ""

        MouseArea {
            anchors.fill: parent
            onClicked: {
                buttonRoot.loadSimulation(simulation)
            }
        }
    }

    Item {
        id: container
        anchors {
            top: nameText.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
            leftMargin: parent.width * 0.05
            topMargin: parent.width * 0.03
            bottomMargin: parent.width * 0.05
            rightMargin: parent.width * 0.05
        }

        Rectangle {
            id: imageContainer
            anchors {
                left: parent.left
                verticalCenter: parent.verticalCenter
            }
            height: parent.height * 0.7
            width: parent.width * 0.45

            color: "black"
            border.color: "#ababab"
            border.width: 1.0

            Image {
                anchors {
                    fill: parent
                    margins: buttonRoot.width * 0.05
                }
                fillMode: Image.PreserveAspectFit
                source: simulation ? simulation.screenshotSource : ""
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    buttonRoot.loadSimulation(simulation)
                }
            }

//            Rectangle {
//                anchors {
//                    left: parent.left
//                    bottom: parent.bottom
//                    right: parent.right
//                    top: startText.top
//                }
//                color: "black"
//                border.color: "#ababab"
//                border.width: 1.0
//            }

            Text {
                id: startText
                anchors {
                    bottom: parent.bottom
                    horizontalCenter: parent.horizontalCenter
                    bottomMargin: parent.height * 0.05
                }
                text: "Tap to start"
                font.capitalization: Font.AllUppercase
                font.pixelSize: buttonRoot.width * 0.02
                font.weight: Font.Light
                color: "#dedede"
            }
        }

//        Flickable {
//            id: descriptionFlickable
//            clip: true
//            width: parent.width * 0.45
//            anchors {
//                top: parent.top
//                bottom: parent.bottom
//                right: parent.right
//            }

//            boundsBehavior: Flickable.StopAtBounds

//            contentWidth: descriptionText.width
//            contentHeight: descriptionText.height

            Text {
                id: descriptionText
                width: parent.width * 0.45
                anchors {
                    top: parent.top
                    bottom: parent.bottom
                    right: parent.right
                }
                color: "#dedede"
                font.pixelSize: buttonRoot.width * 0.03
                font.weight: Font.Light
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                verticalAlignment: Text.AlignVCenter

                text: simulation ? simulation.description : ""
            }

            Text {
                anchors {
                    horizontalCenter: descriptionText.horizontalCenter
                    top: descriptionText.bottom
                }

                color: "#dedede"
                font.pixelSize: buttonRoot.width * 0.02
                font.weight: Font.Light
                font.capitalization: Font.AllUppercase
                text: "Read more"

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        readMore(simulation.description + "\n\n" + simulation.longDescription)
                    }
                }
            }

//        }
    }
}
