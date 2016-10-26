import QtQuick 2.5
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.0
import Atomify 1.0
import "../../plotting"

Column {
    id: fixesColumn
    property bool expanded: false
    height: fixesRow.height + fixesList.height
    
    function create2DPlotWindow(fix, point) {
        console.log("Fix: ", fix)
        var component = Qt.createComponent("../plotting/Plot2D.qml");
        if (component.status == Component.Ready) {
            console.log("Can create now. it is ready")
            var plotter = component.createObject(rectangleRoot);
            plotter.x = point.x - plotter.width*0.5
            plotter.y = point.y - plotter.height*0.5
            plotter.fix = fix
            console.log("Will show")
            plotter.show()
        } else {
            console.log("Error :(")
        }
    }
    
    Row {
        id: fixesRow
        spacing: 2
        height: fixesLabel.height
        
        Image {
            id: collapseFixes
            y: 3
            source: fixesColumn.expanded ? "qrc:/images/collapse.gif" : "qrc:/images/expand.gif"
            MouseArea {
                anchors.fill: parent
                onClicked: fixesColumn.expanded = !fixesColumn.expanded
            }
        }
        Label {
            id: fixesLabel
            text: "Fixes: "+system.fixes.count
            MouseArea {
                anchors.fill: parent
                onClicked: fixesColumn.expanded = !fixesColumn.expanded
            }
        }
    }
    
    ListView {
        id: fixesList
        anchors.top: fixesRow.bottom
        x: fixesLabel.x
        model: system ? system.fixes.model : null
        height: visible ? count*26 : 0
        visible: fixesColumn.expanded
        delegate: Row {
            visible: fixesList.visible
            Label {
                id: fixesTitleLabel
                font.underline: model.modelData.interactive
                color: model.modelData.interactive ? "steelblue" : "white"
                text: {
                    model.modelData.identifier
                }
                MouseArea {
                    anchors.fill: parent
                    cursorShape: model.modelData.interactive ? Qt.PointingHandCursor : Qt.ArrowCursor
                    onClicked: {
                        if(model.modelData.interactive) {
                            var point = Qt.point(mouseX, mouseY)
                            point = getGlobalPosition(point, fixesTitleLabel)
                            create2DPlotWindow(model.modelData, point)
                        }
                    }
                }
            }
        }
    }
}
