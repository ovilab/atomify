import QtQuick 2.7

import "."

QtObject {
    id: root
    signal triggered(var data)
    property string name
    readonly property var connections: Connections {
        target: EventCenter
        onTriggered: {
            if(name === root.name) {
                root.triggered(data)
            }
        }
    }
}
