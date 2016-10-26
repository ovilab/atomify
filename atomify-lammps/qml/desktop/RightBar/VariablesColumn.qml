import QtQuick 2.5
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.0
import Atomify 1.0
import "../../plotting"
Column {
    id: root
    height: row.height + list.height
    property bool expanded: false

    function createPlotWindow(variable, point) {
        var qmlFile = variable.isPerAtom ? "../../plotting/HistogramPlotter.qml" : "../../plotting/VariablePlotter.qml"
        var component = Qt.createComponent(qmlFile);
        if (component.status === Component.Ready) {
            var computePlotter = component.createObject(rectangleRoot);
            computePlotter.x = point.x - computePlotter.width*0.5
            computePlotter.y = point.y - computePlotter.height*0.5
            computePlotter.variable = variable
            computePlotter.show()
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
            text: "Variables: "+system.variables.count
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
        model: system ? system.variables.model : null
        height: visible ? count*20 : 0
        visible: root.expanded
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
