import QtQuick 2.5
import QtGraphicalEffects 1.0

import "qrc:/core"
import "qrc:/mobile/style"


Rectangle {
    id: root
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
                height: root.width > root.height ? root.height * 0.6 : root.height * 0.4

                source: simulation ? simulation.screenshotSource : ""
                smooth: true
                antialiasing: true
                fillMode: Image.PreserveAspectCrop

                MouseArea {
                    anchors.fill: parent
                    onClicked: playClicked(simulation)
                }

                Rectangle {
                    anchors {
                        left: parent.left
                        right: parent.right
                        top: parent.top
                    }
                    height: Style.touchableSize + Style.baseMargin
                    color: "#aa000000"

                    MouseArea {
                        anchors.fill: parent
                    }

                    Image {
                        anchors {
                            left: parent.left
                            top: parent.top
                            bottom: parent.bottom
                            margins: Style.baseMargin / 2
                        }

                        width: height

                        source: "qrc:/images/back.png"
                        smooth: true
                        antialiasing: true

                        MouseArea {
                            anchors.fill: parent
                            onClicked: backClicked()
                        }
                    }
                }

                Image {
                    anchors.centerIn: parent
                    width: Style.touchableSize * 2
                    height: width

                    source: "qrc:/images/play.png"
                    smooth: true
                    antialiasing: true
                }

                LinearGradient {
                    anchors {
                        left: parent.left
                        right: parent.right
                        bottom: parent.bottom
                    }

                    height: Style.touchableSize * 1.5
                    start: Qt.point(0, 0)
                    end: Qt.point(0, height)

                    gradient: Gradient {
                        GradientStop {
                            position: 0
                            color: "#00000000"
                        }
                        GradientStop {
                            position: 1
                            color: "#333333"
                        }
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
                    anchors {
                        left: parent.left
                        right: parent.right
                    }
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
                    textFormat: Text.RichText
                    text: simulation ? simulation.description : "N/A"
                }
            }
            Item {
                anchors {
                    left: parent.left
                    right: parent.right
                }
                height: Style.baseMargin * 4
            }
        }
    }
}

