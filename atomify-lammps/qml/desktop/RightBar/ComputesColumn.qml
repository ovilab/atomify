import QtQuick 2.5
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.0
import QtQuick.Controls 1.4 as QQC1
import Atomify 1.0
import "../../plotting"
Column {
    id: root
    property var system

    function createComputeWindow(compute, point) {
        var qmlFile = compute.isPerAtom ? "../../plotting/HistogramPlotter.qml" : "../../plotting/ComputePlotter.qml"
        var component = Qt.createComponent(qmlFile);
        if (component.status === Component.Ready) {
            console.log("Rectangle root: ", root)
            var computePlotter = component.createObject(root);
            computePlotter.x = point.x - computePlotter.width*0.5
            computePlotter.y = point.y - computePlotter.height*0.5
            computePlotter.compute = compute
            computePlotter.show()
        }
    }
    
    Repeater {
        id: list
        model: system ? system.computes.model : null
        delegate: Row {
            visible: list.visible
            height: 20
            Label {
                id: computeTitleLabel
                property Compute compute: model.modelData
                property int numPerAtomValues: compute ? compute.numPerAtomValues : 0
                property bool hasScalarData: compute ? compute.hasScalarData : false
                font.underline: model.modelData.interactive
                color: model.modelData.interactive ? "steelblue" : "white"
                text: model.modelData ? model.modelData.identifier : ""
                MouseArea {
                    anchors.fill: parent
                    cursorShape: model.modelData.interactive ? Qt.PointingHandCursor : Qt.ArrowCursor
                    hoverEnabled: true
                    onClicked: {
                        if(model.modelData.interactive) {
                            var point = Qt.point(mouseX, mouseY)
                            point = getGlobalPosition(point, computeTitleLabel)
                            createComputeWindow(model.modelData, point)
                        }
                    }
                    onEntered: model.modelData.hovered = true
                    onExited: model.modelData.hovered = false
                }
            }
            Label {
                visible: computeTitleLabel.hasScalarData || computeTitleLabel.numPerAtomValues > 1
                color: (computeTitleLabel.numPerAtomValues > 1) ? "steelblue" : "white"
                text: {
                    if(computeTitleLabel.hasScalarData) {
                        ": "+model.modelData.scalarValue.toFixed(3)
                    } else if(computeTitleLabel.numPerAtomValues > 1) {
                        "   ["+(computeTitleLabel.compute.perAtomIndex+1)+"]"
                    } else ""

                }

                MouseArea {
                    anchors.fill: parent
                    enabled: computeTitleLabel.numPerAtomValues > 1
                    onClicked: menu.open()
                }

                Menu {
                    id: menu

                    Column {
                        Repeater {
                            model: computeTitleLabel.numPerAtomValues
                            MenuItem {
                                text: model.index+1
                                onClicked: {
                                    computeTitleLabel.compute.perAtomIndex = (parseInt(text)-1)
                                    menu.close()
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
