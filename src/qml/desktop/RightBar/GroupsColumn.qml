import QtQuick 2.9
import QtQuick.Controls 2.2

Column {
    Label {
        text: "No groups"
        visible: groupsList.count==0
    }

    Repeater {
        id: groupsList
        model: system ? system.groups.model : undefined
        height: visible ? count*26 : 0
        delegate: Row {
            spacing: 5
            Image {
                id: groupVisible
                width: 15
                height: 15
                y: 1
                source: model.modelData.visible ? "qrc:/images/eye-on.png" : "qrc:/images/eye-off.png"
                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    onHoveredChanged: {
                        model.modelData.hovered = containsMouse
                    }
                    onClicked: model.modelData.visible = !model.modelData.visible
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
                    onClicked: model.modelData.visible = !model.modelData.visible
                    onHoveredChanged: {
                        model.modelData.hovered = containsMouse
                    }
                    cursorShape: Qt.PointingHandCursor
                }
            }
        }
    }
}
