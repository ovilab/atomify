import QtQuick 2.5
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.2
import Atomify 1.0
import "../../plotting"

Column {
    id: root
    property var system
    
    function createPlotWindow(fix, point) {
        var qmlFile = "../../plotting/Plot1D.qml"
        var component = Qt.createComponent(qmlFile);
        if (component.status === Component.Ready) {
            console.log("Can create now. it is ready")
            var plotter = component.createObject(root);
            plotter.x = point.x - plotter.width*0.5
            plotter.y = point.y - plotter.height*0.5
            plotter.control = fix
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
            height: 20
            Label {
                id: titleLabel
                property Fix fix: model.modelData
                property int numPerAtomValues: fix ? fix.numPerAtomValues : 0
                property bool hasScalarData: fix ? fix.hasScalarData : false
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
                            point = getGlobalPosition(point, titleLabel)
                            createPlotWindow(model.modelData, point)
                        }
                    }
                    onEntered: model.modelData.hovered = true
                    onExited: model.modelData.hovered = false
                }
            }
            Label {
                visible: titleLabel.hasScalarData || titleLabel.numPerAtomValues > 1
                color: (titleLabel.numPerAtomValues > 1) ? "steelblue" : "white"
                text: {
                    if(titleLabel.hasScalarData) {
                        if (model.modelData.scalarValue < 0.0005) {
                            ": "+model.modelData.scalarValue.toLocaleString(Qt.locale("en_US"),'e',3);
                        } else {
                            ": "+model.modelData.scalarValue.toPrecision(4) // 4 to be consistent with variables section.
                        }
                    } else if(titleLabel.numPerAtomValues > 1) {
                        "   ["+(titleLabel.fix.perAtomIndex+1)+"]"
                    } else ""

                }

                MouseArea {
                    anchors.fill: parent
                    enabled: titleLabel.numPerAtomValues > 1
                    onClicked: menu.open()
                }

                Menu {
                    id: menu

                    Column {
                        Repeater {
                            model: titleLabel.numPerAtomValues
                            MenuItem {
                                text: model.index+1
                                onClicked: {
                                    titleLabel.fix.perAtomIndex = (parseInt(text)-1)
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
