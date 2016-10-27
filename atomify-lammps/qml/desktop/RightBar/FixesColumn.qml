import QtQuick 2.5
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.0
import Atomify 1.0
import "../../plotting"

Column {
    id: root
    property bool expanded: false
    height: row.height + list.height
    
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
        id: row
        spacing: 2
        height: label.height
        
        Image {
            id: collapse
            y: 3
            source: root.expanded ? "qrc:/images/collapse.gif" : "qrc:/images/expand.gif"
            MouseArea {
                anchors.fill: parent
                onClicked: root.expanded = !root.expanded
            }
        }
        Label {
            id: label
            text: "Fixes: "+system.fixes.count
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
        model: system ? system.fixes.model : null
        height: visible ? count*26 : 0
        visible: root.expanded
        delegate: Row {
            visible: list.visible
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

    Plot2D {
        visible: true
    }
}
