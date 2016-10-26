import QtQuick 2.5
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.0
import Atomify 1.0
import "../../plotting"
Column {
    id: computesColumn
    height: computesRow.height + computesList.height
    property bool expanded: false

    function createComputeWindow(compute, point) {
        var component = Qt.createComponent("../../plotting/ComputePlotter.qml");
        if (component.status == Component.Ready) {
            var computePlotter = component.createObject(rectangleRoot);
            computePlotter.x = point.x - computePlotter.width*0.5
            computePlotter.y = point.y - computePlotter.height*0.5
            computePlotter.compute = compute
            computePlotter.show()
        }
    }
    
    Row {
        id: computesRow
        spacing: 2
        height: computesLabel.height
        
        Image {
            id: collapseComputes
            y: 3
            source: computesColumn.expanded ? "qrc:/images/collapse.gif" : "qrc:/images/expand.gif"
            MouseArea {
                anchors.fill: parent
                onClicked: computesColumn.expanded = !computesColumn.expanded
            }
        }
        Label {
            id: computesLabel
            text: "Computes: "+system.computes.count
            MouseArea {
                anchors.fill: parent
                onClicked: computesColumn.expanded = !computesColumn.expanded
            }
        }
    }
    
    ListView {
        id: computesList
        anchors.top: computesRow.bottom
        x: computesLabel.x
        model: system ? system.computes.model : null
        height: visible ? count*20 : 0
        visible: computesColumn.expanded
        delegate: Row {
            visible: computesList.visible
            Label {
                id: computeTitleLabel
                font.underline: model.modelData.interactive
                color: model.modelData.interactive ? "steelblue" : "white"
                text: model.modelData.identifier
                MouseArea {
                    anchors.fill: parent
                    cursorShape: model.modelData.interactive ? Qt.PointingHandCursor : Qt.ArrowCursor
                    onClicked: {
                        if(model.modelData.interactive) {
                            var point = Qt.point(mouseX, mouseY)
                            point = getGlobalPosition(point, computeTitleLabel)
                            createComputeWindow(model.modelData, point)
                        }
                    }
                }
            }
            Label {
                text: {
                    if(model.modelData.hasScalarData) {
                        ": "+model.modelData.scalarValue.toFixed(3)
                    } else {
                        ""
                    }
                }
            }
        }
    }
}
