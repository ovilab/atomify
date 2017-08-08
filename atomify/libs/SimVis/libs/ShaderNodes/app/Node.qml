import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.Controls.Material 2.0
import QtGraphicalEffects 1.0

import ShaderNodes 1.0
import ShaderNodesApp 1.0

Item {
    id: root

    signal dropReceived(var from, var to)
    signal clicked

    property var inputHandles: []
    property var outputHandles: []
    property var allHandles: []
    property var arrayProperties: []
    property string identifier
    property string exportedTypeName
    property string name
    property string title: exportedTypeName ? exportedTypeName : name
    property bool virtualized: exportedTypeName.length < 1
    property bool selected: false

    property alias source: loader.source
    property ShaderNode shaderNode: loader.item ? loader.item : null
    property ShaderNode originalNode // used when loading
    property alias nodeWrapper: nodeWrapper

    width: 360
    height: Math.max(inputColumn.y + inputColumn.height, outputColumn.y + outputColumn.height) + 24

    Component.onCompleted: {
        parseNode(shaderNode)
    }

    onXChanged: {
        updateHandleConnectionPoints()
    }

    onYChanged: {
        updateHandleConnectionPoints()
    }

    onWidthChanged: {
        updateHandleConnectionPoints()
    }

    onHeightChanged: {
        updateHandleConnectionPoints()
    }

    function reloadShaderNode() {
//        var tmp = loader.source
//        loader.source = ""
//        loader.source = tmp
    }

    function updateHandleConnectionPoints() {
        for(var i in allHandles) {
            var handle = allHandles[i]
            handle.connectionPoint = parent.mapFromItem(handle, handle.plug.x + handle.plug.width / 2, handle.plug.y + handle.plug.height / 2)
        }
    }

    function createInputHandle(name, value) {
        var handleComponent = Qt.createComponent("Handle.qml")
        if(handleComponent.status !== Component.Ready) {
            console.log(handleComponent.errorString())
            return
        }
//        var glslType = ShaderNodes.glslType(value)
        var properties = {
            name: name,
            identifier: name,
            type: "input",
            node: root,
            value: value,
            defaultValue: value
        }
        var handle = handleComponent.createObject(inputColumn, properties)
        handle.dropReceived.connect(function(from) {
            root.dropReceived(from, handle)
        })
        handle.newHandleClicked.connect(function() {
            addAnotherHandle(handle)
        })
        inputHandles.push(handle)
        allHandles = inputHandles.concat(outputHandles)
        updateHandleConnectionPoints()
        return handle
    }

    function addAnotherHandle(handle) {
        var newHandle = createInputHandle(handle.name, handle.value)
        newHandle.arrayBased = true
        return newHandle
    }

    function parseNode(shaderNode) {
        if(!shaderNode) {
            return
        }

        root.exportedTypeName = shaderNode.exportedTypeName
        root.name = shaderNode.name
        root.arrayProperties = shaderNode.arrayProperties

        for(var i in inputHandles) {
            var handle = inputHandles[i]
            handle.destroy()
        }
        for(var i in outputHandles) {
            var handle = outputHandles[i]
            handle.destroy()
        }

        var names = shaderNode.inputNames()

        for(var i in names) {
            var name = names[i]
            var handle = createInputHandle(name, shaderNode[name])
            if(shaderNode.arrayProperties.indexOf(name) > -1) {
                handle.arrayBased = true
            }
        }

        var newOutputHandles = []


        var handleComponent = Qt.createComponent("Handle.qml")
        if(handleComponent.status !== Component.Ready) {
            console.log(handleComponent.errorString())
            return
        }

        var outputHandle = handleComponent.createObject(outputColumn, {name: "result", type: "output", node: root})
        newOutputHandles.push(outputHandle)

        outputHandles = newOutputHandles

        allHandles = inputHandles.concat(outputHandles)

//        object.destroy()

        updateHandleConnectionPoints()
    }

    NodeWrapper {
        id: nodeWrapper
        shaderNode: root.shaderNode
    }

    Loader {
        id: loader
    }

    Rectangle {
        id: background
        anchors.fill: parent
        radius: 6.0
        color: Material.background
    }

    DropShadow {
        id: dropShadow
        anchors.fill: background
        source: background
        radius: 12
        verticalOffset: 4
        horizontalOffset: 2
        samples: 16
        color: "#FF000000"
        smooth: true
        antialiasing: true

        states: [
            State {
                when: selected
                PropertyChanges {
                    target: dropShadow
                    verticalOffset: 0
                    horizontalOffset: 0
                    color: "#FF117799"
                }
            }
        ]

        transitions: [
            Transition {
                from: "*"
                to: "*"
                PropertyAnimation {
                    properties: "color,verticalOffset,horizontalOffset"
                    duration: 120
                    easing.type: Easing.InOutQuad
                }
            }
        ]
    }

    MouseArea {
        anchors.fill: parent
        drag.target: parent
        drag.threshold: 4
        onClicked: root.clicked()
    }

    Column {
        id: inputColumn
        anchors {
            left: parent.left
            top: titleText.bottom
            margins: 8
        }
        spacing: 8
    }

    Column {
        id: outputColumn
        anchors {
            right: parent.right
            top: titleText.bottom
            margins: 8
        }
        spacing: 8
    }

    Label {
        id: titleText
        anchors {
            margins: 8
            top: parent.top
            horizontalCenter: parent.horizontalCenter
        }
        text: title
    }
}
