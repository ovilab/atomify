import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import Atomify 1.0
import SimVis 1.0

import "../visualization"
import ".."
ScrollView {
    id: renderingRoot
    anchors.fill: parent
    property AtomifyVisualizer atomifyVisualizer
    property System system
    property var items: []


    contentItem: Column {
        id: column
        anchors.fill: parent

        ComboBox {
            model: [ "Add compute", "Pair distribution function", "Energy" ]
            onCurrentIndexChanged: {
                if(currentText === "Pair distribution function") {
                    addMeasure("../RDFItem.qml")
                }

                currentIndex = 0
                focus = false
            }
        }

        Grid {
            id: grid
            width: Math.max(renderingRoot.viewport.width, 300)
            columns: 2
            function updateColumns() {
                if(children.length === 1) {
                    columns = 1
                } else {
                    if(width < 800) {
                        columns = 1
                    } else columns = 2
                }
                console.log("Columns: ", columns)
            }

            onWidthChanged: {
                console.log("Width changed: ", width)
                updateColumns()
            }

            onChildrenChanged: {
                console.log("Children changed")
                updateColumns()
            }

        }
    }

    function addMeasure(qmlFile) {
        var component = Qt.createComponent(qmlFile);
        if (component.status === Component.Ready) {
            var QMLObject = component.createObject(grid, {});
            if(QMLObject===null) {
                console.log("Could not add...")
            } else {
                QMLObject.system = system

                items.push(QMLObject)
                itemsChanged(items)
            }
        } else {
            console.log("Could not add...")
        }
    }

//    Button {
//        text: "Add"
//        onClicked: {
//            addThing()
//        }
//    }
}

