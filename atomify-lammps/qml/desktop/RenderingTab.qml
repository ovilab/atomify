import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import Atomify 1.0
import SimVis 1.0

import "../visualization"
import ".."
Item {
    id: renderingRoot
    anchors.fill: parent
    property AtomifyVisualizer atomifyVisualizer
    property System system
    property var items: []

    ColumnLayout {
        anchors.fill: parent

        ComboBox {
            model: [ "Add compute", "RDF", "Energy" ]
            onCurrentIndexChanged: {
                if(currentText === "RDF") {
                    addMeasure("../RDFItem.qml")
                    currentIndex = 0
                }
            }
        }

        GridLayout {
            id: grid
            // Layout.fillHeight: true
            Layout.fillWidth: true
            columns: 2
        }
    }

    function addMeasure(qmlFile) {
        var component = Qt.createComponent(qmlFile);
        if (component.status === Component.Ready) {
            var QMLObject = component.createObject(grid, {});
            if(QMLObject===null) {
                console.log("Could not add...")
            } else {
                // QMLObject.parent = grid
                QMLObject.system = system
                QMLObject.Layout.fillWidth = true
                QMLObject.Layout.fillHeight = true
                QMLObject.Layout.preferredWidth = 1
                QMLObject.Layout.preferredHeight = 1
                QMLObject.Layout.maximumHeight = 100

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

