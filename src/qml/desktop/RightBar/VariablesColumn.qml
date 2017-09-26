import QtQuick 2.5
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.2
import Atomify 1.0
import "../../plotting"
Column {
    id: root
    property var system

    function createPlotWindow(variable, point) {
        var qmlFile = "../../plotting/Plot1D.qml"
        var component = Qt.createComponent(qmlFile);
        if (component.status === Component.Ready) {
            var plotter = component.createObject(root);
            plotter.x = point.x - plotter.width*0.5
            plotter.y = point.y - plotter.height*0.5
            plotter.control = variable
            plotter.show()
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
                    var value = model.modelData.scalarValue
                    var valueHasDecimals = value !== Math.round(value, 0)
                    if(valueHasDecimals) {
                        if (value < 0.0005) {
                            ": "+value.toLocaleString(Qt.locale("en_US"),'e',3);
                        } else {
                            ": "+value.toPrecision(4)
                        }
                    } else {
                        ": "+value.toFixed(0)
                    }
                }
            }
        }
    }
}

