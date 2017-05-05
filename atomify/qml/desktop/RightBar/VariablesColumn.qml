import QtQuick 2.5
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.0
import Atomify 1.0
import "../../plotting"
Column {
    id: root
    property var system

    function createPlotWindow(variable, point) {
        var qmlFile = variable.isPerAtom ? "../../plotting/HistogramPlotter.qml" : "../../plotting/VariablePlotter.qml"
        var component = Qt.createComponent(qmlFile);
        if (component.status === Component.Ready) {
            var computePlotter = component.createObject(root);
            computePlotter.x = point.x - computePlotter.width*0.5
            computePlotter.y = point.y - computePlotter.height*0.5
            computePlotter.variable = variable
            computePlotter.show()
        }
    }

    Label {
        text: "No variables"
        visible: list.count==0
    }

    Repeater {
        id: list
        model: system ? system.variables.model : null
        delegate: Row {
            visible: list.visible
            Label {
                id: titleLabel
                font.underline: true
                color: "steelblue"
                text: model.modelData.identifier
                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    hoverEnabled: true
                    onClicked: {
                        var point = Qt.point(mouseX, mouseY)
                        point = getGlobalPosition(point, titleLabel)
                        createPlotWindow(model.modelData, point)
                    }
                    onEntered: model.modelData.hovered = true
                    onExited: model.modelData.hovered = false
                }
            }
            Label {
                text: {
                    if(model.modelData.isPerAtom) return ""

                    if(model.modelData.valueHasDecimals) {
                        ": "+model.modelData.value.toFixed(3)
                    } else {
                        ": "+model.modelData.value.toFixed(0)
                    }
                }
            }
        }
    }
}
