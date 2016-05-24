import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import Atomify 1.0
import SimVis 1.0

import "../visualization"
import ".."
Item {
    id: renderingRoot
    property AtomifyVisualizer atomifyVisualizer
    property var items: []

    Grid {
        id: grid
        width: parent.width
        columns: 2
    }

    function addThing() {
        var component = Qt.createComponent("../RDFItem.qml");
        if (component.status === Component.Ready) {
            var QMLObject = component.createObject(null, {});
            if(QMLObject===null) {
                console.log("Could not add RDF...")
            } else {
                QMLObject.parent = grid
                QMLObject.width = grid.width/2
                QMLObject.height = grid.width/2
                items.push(QMLObject)
                console.log("doing items changed")
                itemsChanged(items)
            }
        } else {
            console.log("Could not add RDF2...")
        }
    }

    Button {
        text: "Add"
        onClicked: {
            addThing()
        }
    }
}

