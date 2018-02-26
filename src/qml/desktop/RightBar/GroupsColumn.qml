import QtQuick 2.9
import QtQuick.Controls 2.2
import "../../visualization"

Column {
    ColorList { id: colorList }

    Label {
        text: "No groups"
        visible: groupsList.count==0
    }

    Repeater {
        id: groupsList
        model: system ? system.groups.model : undefined
        delegate: Row {
            id: groupRow
            spacing: 5
            state: "Normal"

            function hovered(containsMouse) {
                model.modelData.hovered = containsMouse;
            }

            function clicked() {
                if (groupRow.state === "Normal") {
                    groupRow.state = "Always"
                } else if (groupRow.state === "Always") {
                    groupRow.state = "Hide"
                } else if (groupRow.state === "Hide") {
                    groupRow.state = "Normal"
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
                    onHoveredChanged: groupRow.hovered(containsMouse)
                    onClicked: groupRow.clicked()
                    cursorShape: Qt.PointingHandCursor
                }
            }
            Label {
                id: groupLabel
                visible: groupsList.visible
                text: model.modelData.identifier+": "+model.modelData.count+" atoms"
                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    onHoveredChanged: groupRow.hovered(containsMouse)
                    onClicked: groupRow.clicked()
                    cursorShape: Qt.PointingHandCursor
                }
            }
        }
    }
}
