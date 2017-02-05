import QtQuick 2.5
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.1
import Atomify 1.0
import "../../plotting"

Column {
    id: root
    property var system
    
    function create2DPlotWindow(fix, point) {
        var component = Qt.createComponent("../../plotting/Plot2D.qml");
        if (component.status === Component.Ready) {
            console.log("Can create now. it is ready")
            var plotter = component.createObject(root);
            plotter.x = point.x - plotter.width*0.5
            plotter.y = point.y - plotter.height*0.5
            plotter.fix = fix
            console.log("Will show")
            plotter.show()
        } else {
            console.log("Error :(")
        }
    }

    Label {
        text: "No fixes"
        visible: list.count==0
    }
    
    Repeater {
        id: list
        model: system ? system.fixes.model : null
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
}
