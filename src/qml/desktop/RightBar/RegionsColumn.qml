import QtQuick 2.5
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.2
import Atomify 1.0
import "../../plotting"
import "../../visualization"

Column {
    id: root
    property var system

    ColorList { id: colorList }

    Label {
        text: "No regions"
        visible: list.count==0
    }

    Repeater {
        id: list
        model: system ? system.regions.model : undefined
        delegate: Row {
            id: regionsRow
            spacing: 5
            state: "Normal"

            function hovered(containsMouse) {
                model.modelData.hovered = containsMouse;
            }

            function clicked() {
                if (regionsRow.state === "Normal") {
                    regionsRow.state = "Always"
                } else if (regionsRow.state === "Always") {
                    regionsRow.state = "Hide"
                } else if (regionsRow.state === "Hide") {
                    regionsRow.state = "Normal"
                }
            }

            states: [
                State {
                    name: "Normal"
                    PropertyChanges { target: model.modelData; visible: true }
                    PropertyChanges { target: model.modelData; marked: false }
                    PropertyChanges { target: colorRect; color: colorList.colorForIndex(index) }
                    PropertyChanges { target: colorRect; border.width: 0 }
                },
                State {
                    name: "Hide"
                    PropertyChanges { target: model.modelData; visible: false }
                    PropertyChanges { target: model.modelData; marked: false }
                    PropertyChanges { target: colorRect; color: colorList.dampedColorForIndex(index) }
                    PropertyChanges { target: colorRect; border.width: 0 }
                },
                State {
                    name: "Always"
                    PropertyChanges { target: model.modelData; visible: true }
                    PropertyChanges { target: model.modelData; marked: true }
                    PropertyChanges { target: colorRect; color: colorList.colorForIndex(index) }
                    PropertyChanges { target: colorRect; border.width: 1 }
                }
            ]

            Rectangle {
                id: colorRect
                width: 15
                height: 15
                border.color: "white"

                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    onHoveredChanged: regionsRow.hovered(containsMouse)
                    onClicked: regionsRow.clicked()
                    cursorShape: Qt.PointingHandCursor
                }
            }

            Label {
                visible: list.visible
                text: model.modelData.identifier+": "+model.modelData.count+" atoms"
                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    onHoveredChanged: regionsRow.hovered(containsMouse)
                    onClicked: regionsRow.clicked()
                    cursorShape: Qt.PointingHandCursor
                }
            }
            
        }
    }
}
