import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0

import Atomify 1.0

import "qrc:/core"
import "qrc:/mobile/style"
import "qrc:/visualization"

Rectangle {
    id: viewRoot

    signal backClicked

    property AtomifyVisualizer visualizer
    property AtomifySimulator simulator
    property Simulation simulation
    property int currentIndex

    width: parent.width
    height: parent.height
    color: "#222"

    function gotoIndex(index) {
        flickable.contentX = flickable.width * index
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

    Row {
        id: controlList
        anchors {
            top: backButton.bottom
            left: parent.left
            right: parent.right
            margins: Style.baseMargin
        }
        Repeater {
            model: simulation ? simulation.controllers.length : undefined
            Rectangle {
                width: 120
                height: 48
                color: currentIndex === index ? "white" : "transparent"
                border {
                    color: "white"
                    width: 2.0
                }
                Text {
                    text: simulation.controllers[index].name
                    color: currentIndex === index ? "black" : Style.font.color
                    anchors.centerIn: parent
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked: currentIndex = index
                }
            }
        }
    }

    Repeater {
        model: simulation.controllers.length
        Loader {
            anchors {
                left: parent.left
                right: parent.right
                top: controlList.bottom
                bottom: parent.bottom
                margins: Style.baseMargin
            }
            visible: currentIndex === index

            sourceComponent: simulation.controllers[index].fullControl
        }
    }

//    Flickable {
//        id: flickable
//        anchors {
//            top: controlList.bottom
//            left: parent.left
//            right: parent.right
//            bottom: parent.bottom
//        }

//        contentHeight: controlContainer.height
//        contentWidth: controlContainer.width
//        flickableDirection: Flickable.HorizontalFlick

//        RowLayout {
//            id: controlContainer

//            property real itemWidth: flickable.width
//            property real itemHeight: flickable.height

//            width: simulation ? simulation.controllers.length * itemWidth : 0
//            height: itemHeight

//            Repeater {
//                id: repeater
//                model: simulation ? simulation.controllers.length : undefined
//                Loader {
//                    id: miniControlLoader
//                    Layout.fillHeight: true
//                    Layout.preferredWidth: controlContainer.itemWidth

//                    sourceComponent: simulation ? simulation.controllers[index].fullControl : undefined

//                    onLoaded: {
//                        console.log("Loaded fullControl")
//                        if(!item || !visualizer) {
//                            return
//                        }

//                        item.visualizer = visualizer
//                    }
//                }
//            }
//        }
//    }
}

