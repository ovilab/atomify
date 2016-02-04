import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1

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

    Flickable {
        id: flickable
        anchors {
            top: backButton.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }

        contentHeight: controlContainer.height
        contentWidth: controlContainer.width
        flickableDirection: Flickable.HorizontalFlick

        RowLayout {
            id: controlContainer

            property real itemWidth: flickable.width
            property real itemHeight: flickable.height

            width: simulation.controllers.length * itemWidth
            height: itemHeight

            Repeater {
                id: repeater
                model: simulation.controllers.length
                Loader {
                    id: miniControlLoader
                    Layout.fillHeight: true
                    Layout.preferredWidth: controlContainer.itemWidth

                    sourceComponent: simulation.controllers[index].fullControl

                    onLoaded: {
                        console.log("Loaded fullControl")
                        if(!item || !visualizer) {
                            return
                        }

                        item.visualizer = visualizer
                    }
                }
            }
        }
    }
}

