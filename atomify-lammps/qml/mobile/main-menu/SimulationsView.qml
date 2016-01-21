import QtQuick 2.5
import "qrc:/mobile/style"

Item {
    id: simulationSelectionView
    property alias model: listView.model
    property string title: "Select simulation"
    signal loadSimulation(var simulation)
    signal readMore(var text)

    ListModel {
        id: simulationsModel
        ListElement {
            folder: "../../simulations/diffusion/simple_diffusion"
        }
        ListElement {
            folder: "../../simulations/water/singlewater"
        }
        ListElement {
            folder: "../../simulations/other/indent"
        }
    }

    ListView {
        id: listView
        anchors {
            top: parent.top
            bottom: dots.top
            left: parent.left
            right: parent.right
            bottomMargin: Style.touchableSize * 0.1
        }

        orientation: Qt.Horizontal
        snapMode: ListView.SnapOneItem
        model: simulationsModel
        cacheBuffer: 999999

        onContentXChanged: {
            dotsRepeater.currentIndex = indexAt(contentX + width / 2, contentY)
        }

        delegate: SimulationButton {
            width: listView.width
            height: listView.height
            folder: model.folder
            onLoadSimulation: {
                simulationSelectionView.loadSimulation(simulation)
            }
            onReadMore: {
                simulationSelectionView.readMore(text)
            }
        }
    }

    Row {
        id: dots
        anchors {
            bottom: parent.bottom
            bottomMargin: Style.touchableSize * 0.7
            horizontalCenter: parent.horizontalCenter
        }
        height: Style.baseSize * 1.2
        spacing: Style.baseSize * 0.8

        Repeater {
            id: dotsRepeater
            property int currentIndex: 0
            model: simulationsModel.count

            function refresh() {
                for(var i = 0; i < count; i++) {
                    itemAt(i).selected = false
                }
                if(itemAt(currentIndex)) {
                    itemAt(currentIndex).selected = true
                }
            }

            Component.onCompleted: {
                refresh()
            }

            onCurrentIndexChanged: {
                refresh()
            }

            Rectangle {
                id: dot
                property bool selected: false
                anchors.verticalCenter: parent.verticalCenter
                width: Style.baseSize * 0.8
                height: width
                color: "grey"

                radius: width / 2

                states: [
                    State {
                        name: "selected"
                        when: selected
                        PropertyChanges {
                            target: dot
                            color: "white"
                        }
                        PropertyChanges {
                            target: dot
                            width: Style.baseSize * 1.2
                        }
                    }
                ]

                transitions: [
                    Transition {
                        ParallelAnimation {
                            NumberAnimation {
                                target: dot
                                property: "width"
                                duration: 400
                                easing.type: Easing.InOutQuad
                            }
                            NumberAnimation {
                                target: dot
                                property: "height"
                                duration: 400
                                easing.type: Easing.InOutQuad
                            }
                            ColorAnimation {
                                duration: 400
                            }
                        }
                    }
                ]
            }
        }
    }
}

