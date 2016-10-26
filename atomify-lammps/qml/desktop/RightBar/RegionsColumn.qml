import QtQuick 2.5
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.0
import Atomify 1.0
import "../../plotting"

Column {
    id: root
    property bool expanded
    height: row.height + list.height
    
    Row {
        id: row
        spacing: 2
        height: regionsLabel.height
        
        Image {
            id: collapseRegions
            y: 3
            source: root.expanded ? "qrc:/images/collapse.gif" : "qrc:/images/expand.gif"
            MouseArea {
                anchors.fill: parent
                onClicked: root.expanded = !root.expanded
            }
        }
        Label {
            id: regionsLabel
            text: "Regions: "+system.regions.count
            MouseArea {
                anchors.fill: parent
                onClicked: root.expanded = !root.expanded
            }
        }
    }
    
    ListView {
        id: list
        anchors.top: row.bottom
        x: regionsLabel.x
        model: system ? system.regions.model : null
        height: visible ? count*26 : 0
        visible: root.expanded
        delegate: Row {
            spacing: 5
            Image {
                id: regionVisible
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
                visible: list.visible
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
