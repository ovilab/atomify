import QtQuick 2.5
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.0
import Atomify 1.0
import "../../plotting"
Column {
    id: root
    height: row.height + list.height
    property bool expanded: false

    function createComputeWindow(compute, point, isPerAtom) {
        var qmlFile = isPerAtom ? "../../plotting/HistogramPlotter.qml" : "../../plotting/ComputePlotter.qml"
        var component = Qt.createComponent(qmlFile);
        if (component.status === Component.Ready) {
            console.log("Rectangle root: ", rectangleRoot)
            var computePlotter = component.createObject(rectangleRoot);
            computePlotter.x = point.x - computePlotter.width*0.5
            computePlotter.y = point.y - computePlotter.height*0.5
            computePlotter.compute = compute
            computePlotter.show()
        }
    }
    
    Row {
        id: row
        spacing: 2
        height: label.height
        
        Image {
            id: collapseComputes
            y: 3
            source: root.expanded ? "qrc:/images/collapse.gif" : "qrc:/images/expand.gif"
            MouseArea {
                anchors.fill: parent
                onClicked: root.expanded = !root.expanded
            }
        }
        Label {
            id: label
            text: "Computes: "+system.computes.count
            MouseArea {
                anchors.fill: parent
                onClicked: root.expanded = !root.expanded
            }
        }
    }
    
    ListView {
        id: list
        anchors.top: row.bottom
        x: label.x
        model: system ? system.computes.model : null
        height: visible ? count*20 : 0
        visible: root.expanded
        delegate: Row {
            visible: list.visible
            Label {
                id: computeTitleLabel
                font.underline: model.modelData.interactive
                color: model.modelData.interactive ? "steelblue" : "white"
                text: model.modelData.identifier
                MouseArea {
                    anchors.fill: parent
                    cursorShape: model.modelData.interactive ? Qt.PointingHandCursor : Qt.ArrowCursor
                    hoverEnabled: true
                    onClicked: {
                        if(model.modelData.interactive) {
                            var point = Qt.point(mouseX, mouseY)
                            point = getGlobalPosition(point, computeTitleLabel)
                            createComputeWindow(model.modelData, point, model.modelData.isPerAtom)
                        }
                    }
                    onEntered: model.modelData.hovered = true
                    onExited: model.modelData.hovered = false
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

//    HistogramPlotter {
//        id: hist
//        visible: true
//    }
}
