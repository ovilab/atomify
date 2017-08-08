import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.Controls.Material 2.0

import ShaderNodesApp 1.0

Item {
    id: root

    signal newHandleClicked
    signal dropReceived(var from)

    property alias plug: plug

    property bool occupied: false
    property string name
    property string identifier
    property string type: "input"
    property Node node
    property point connectionPoint: Qt.point(500, 500)
    property int index: -1
    property bool arrayBased: false
    property var value
    property var defaultValue

    width: 120
    height: 24

    onValueChanged: {
        if(occupied) {
           return
        }
        node.shaderNode[identifier] = Qt.binding(function() {return value})
    }

    function reset() {
        value = defaultValue
        node.nodeWrapper.resetProperty(name)
    }

    Plug {
        id: plug

        anchors {
            left: type === "input" ? parent.left : undefined
            right: type === "input" ? undefined : parent.right
        }

        height: parent.height
        color: occupied ? Material.shade(Material.accent, Material.Shade600) : Material.shade(Material.accent, Material.Shade200)
    }

    Label {
        id: nameText
        anchors {
            left: type === "input" ? plug.right : undefined
            right: type === "input" ? undefined : plug.left
            verticalCenter: parent.verticalCenter
            margins: 8
        }

        text: name
    }

    MouseArea {
        id: mouseArea

        enabled: root.type === "output"

        anchors.fill: parent
        drag.target: dragHandle
        drag.threshold: 0

        onReleased: dragHandle.Drag.drop()
    }

    Rectangle {
        anchors {
            left: nameText.right
            verticalCenter: nameText.verticalCenter
        }

        visible: type === "input" && arrayBased
        width: 24
        height: 24

        MouseArea {
            anchors.fill: parent
            onClicked: {
                newHandleClicked()
            }
        }
        Text {
            anchors.centerIn: parent
            text: "+"
        }
    }

    DropArea {
        id: dragTarget

        anchors.fill: parent
        enabled: type === "input"
        keys: ["handle", "edge"]
        states: [
            State {
                when: dragTarget.containsDrag
                PropertyChanges {
                    target: plug
                    color: "grey"
                }
            }
        ]
        onDropped: {
            if(drag.source.handle.node === root.node) {
                console.log("Cannot connect to the same node")
                return
            }
            if(drop.keys.indexOf("edge") > -1) {
                drag.source.edge.reconnectTo(root)
            } else {
                dropReceived(drag.source.handle)
            }
        }
    }

    SCurve {
        id: sCurve
        startPoint: root.connectionPoint
        endPoint: Qt.point(dragHandle.x + dragHandle.width / 2, dragHandle.y + dragHandle.height / 2)
        color: Material.foreground
        parent: node.parent
        visible: false

        states: State {
            when: mouseArea.drag.active
//            ParentChange { target: sCurve; parent: node.parent }
            PropertyChanges { target: sCurve; visible: true }
        }
    }

    Item {
        id: dragHandle

        property var handle: root

        anchors {
            verticalCenter: plug.verticalCenter
            horizontalCenter: plug.horizontalCenter
        }

        width: 24
        height: width
        visible: type === "output"

        Drag.keys: ["handle"]
        Drag.active: mouseArea.drag.active
        Drag.hotSpot.x: width / 2
        Drag.hotSpot.y: height / 2

        states: State {
            when: mouseArea.drag.active
            ParentChange { target: dragHandle; parent: node.parent }
            AnchorChanges { target: dragHandle; anchors.verticalCenter: undefined; anchors.horizontalCenter: undefined }
        }
    }
}
