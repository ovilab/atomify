import QtQuick 2.5
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.0
import Atomify 1.0
import "../../plotting"

Column {
    id: regionsColumn
    property bool expanded
    height: regionsRow.height + regionsList.height
    
    Row {
        id: regionsRow
        spacing: 2
        height: regionsLabel.height
        
        Image {
            id: collapseRegions
            y: 3
            source: regionsColumn.expanded ? "qrc:/images/collapse.gif" : "qrc:/images/expand.gif"
            MouseArea {
                anchors.fill: parent
                onClicked: regionsColumn.expanded = !regionsColumn.expanded
            }
        }
        Label {
            id: regionsLabel
            text: "Regions: "+system.regions.count
            MouseArea {
                anchors.fill: parent
                onClicked: regionsColumn.expanded = !regionsColumn.expanded
            }
        }
    }
    
    ListView {
        id: regionsList
        anchors.top: regionsRow.bottom
        x: regionsLabel.x
        model: system ? system.regions.model : null
        height: visible ? count*26 : 0
        visible: regionsColumn.expanded
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
                visible: regionsList.visible
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
