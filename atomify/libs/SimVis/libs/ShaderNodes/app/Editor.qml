import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Material 2.0

import ShaderNodes 1.0

Rectangle {
    id: root

    property var nodes: []
    property var edges: []
    property Node finalNode
    property Node activeNode

    property var outputNode

    property ShaderBuilderMaterial shaderBuilderMaterial
    property ShaderBuilder shaderBuilder: shaderBuilderMaterial.fragment
    property int indentLevel: 0
    property string output

    focus: true
    clip: true
    color: Material.shade(Material.background, Material.Shade100)

    Component.onCompleted: {
        var offset = 100
        for(var i in shaderBuilder.inputs) {
            var input = shaderBuilder.inputs[i]
            var node = createInputNode(input, {x: 10, y: 10 + offset, identifier: "shaderBuilder.inputs[" + i + "]"})
            offset += 100
        }

        var mix = createNode("/ShaderNodes/Mix.qml", {x: 400, y: 10})
        var standardMaterial = createNode("/ShaderNodes/StandardMaterial.qml", {x: 800, y: 400})

        finalNode = standardMaterial
        activeNode = finalNode

        createEdge(nodes[0].outputHandles[0], mix.inputHandles[0])
        createEdge(nodes[1].outputHandles[0], mix.inputHandles[1])
        createEdge(mix.outputHandles[0], standardMaterial.inputHandles[0])

        outputNode = nodes[2]

        refreshOutput()
        shaderBuilderMaterial.fragmentColor = finalNode.shaderNode // TODO do somewhere else
    }

    function logOutput() {
        refreshOutput()
        console.log(output)
    }

    function deleteSelected() {
        var nodesToDelete = []
        for(var i in nodes) {
            var node = nodes[i]
            if(node.selected) {
                nodesToDelete.push(node)
            }
        }
        for(var i in nodesToDelete) {
            var node = nodesToDelete[i]
            deleteNode(node)
        }
    }

    function forceDeleteNode(node) {
        var edgesToDelete = []
        for(var i in edges) {
            var edge = edges[i]
            if(edge.to.node === node || edge.from.node === node) {
                edgesToDelete.push(edge)
            }
        }
        for(var i in edgesToDelete)  {
            var edge = edgesToDelete[i]
            deleteEdge(edge)
        }
        nodes.splice(nodes.indexOf(node), 1)
        refreshValues()
        node.destroy()
    }

    function deleteNode(node) {
        if(node === finalNode) {
            popup.show("Cannot delete final node")
            return
        }
        forceDeleteNode(node)
    }

    function removeOtherEdges(myEdge) {
        var edgesToDelete = []
        for(var i in edges) {
            var edge = edges[i]
            if(edge.to === myEdge.to && edge !== myEdge) {
                edgesToDelete.push(edge)
            }
        }
        for(var i in edgesToDelete) {
            edge = edgesToDelete[i]
            deleteEdge(edge)
        }
    }

    function deleteEdge(edge) {
        edges.splice(edges.indexOf(edge), 1)
        refreshValues()
        refreshOccupation()
        edge.destroy()
    }

    function createHandleBinding(handle){
        return Qt.binding(function() {
            return handle.value
        })
    }

    function refreshValues() {
        for(var i in nodes) {
            var node = nodes[i]
            var foundHandles = []
            var arrayHandles = {}
            for(var k in node.inputHandles) {
                var handle = node.inputHandles[k]
                if(handle.arrayBased) {
                    var found = false
                    for(var j in edges) {
                        var edge = edges[j]
                        var fromShaderNode = edge.from.node.shaderNode
                        if(edge.to === handle) {
                            if(arrayHandles[handle.identifier]) {
                                arrayHandles[handle.identifier].push(fromShaderNode)
                            } else {
                                arrayHandles[handle.identifier] = [fromShaderNode]
                            }
                            found = true
                            break
                        }
                    }
                    if(!found) {
                        if(handle.value === handle.defaultValue) {
                            handle.reset()
                        } else {
                            var serializedValue = handle.value
                            if(arrayHandles[handle.identifier]) {
                                arrayHandles[handle.identifier].push(serializedValue)
                            } else {
                                arrayHandles[handle.identifier] = [serializedValue]
                            }
                        }
                    }
                } else {
                    var found = false
                    for(var j in edges) {
                        var edge = edges[j]
                        if(edge.to === handle) {
                            handle.node.shaderNode[handle.identifier] = edge.from.node.shaderNode
                            handle.node.shaderNode.markDirty() // TODO this shouldn't be necessary
                            found = true
                            break
                        }
                    }
                    if(!found) {
                        if(handle.value === handle.defaultValue) {
                            handle.reset()
                        } else {
                            handle.node.shaderNode[handle.identifier] = handle.value
                        }
                    }
                }
            }

            for(var handleName in arrayHandles) {
                node.shaderNode[handleName] = arrayHandles[handleName]
            }
        }
    }

    function refreshOccupation() {
        for(var i in nodes) {
            var node = nodes[i]
            for(var j in node.allHandles) {
                var handle = node.allHandles[j]
                handle.occupied = false
            }
        }
        for(var i in edges) {
            var edge = edges[i]
            edge.from.occupied = true
            edge.to.occupied = true
        }
    }

    function createEdge(from, to) {
        var edgeComponent = Qt.createComponent("Edge.qml")
        if(edgeComponent.status !== Component.Ready) {
            console.log(edgeComponent.errorString())
            return
        }

        var edge = edgeComponent.createObject(workspace, {from: from, to: to})

        edge.droppedNowhere.connect(function() {
            deleteEdge(edge)
            refreshOutput()
            refreshOccupation()
            refreshValues()
        })

        edge.reconnectTo.connect(function(to) {
            edge.dropCaught = true
            edge.to.occupied = false
            edge.to = to
            edge.to.occupied = true
            removeOtherEdges(edge)
            refreshOutput()
            refreshOccupation()
            refreshValues()
        })

        edges.push(edge)
        refreshOccupation()
        refreshValues()
        return edge
    }

    function deselectAll() {
        for(var i in nodes) {
            var otherNode = nodes[i]
            otherNode.selected = false
        }
        activeNode = null
    }

    function createNode(source, properties) {
        properties.source = source
        var node = createNodeFromComponent(properties)
        node.shaderNode.parent = shaderBuilderMaterial // necessary for texture nodes
        finalizeCreatedNode(node)
        return node
    }

    function createInputNode(shaderNode, properties) {
        properties.shaderNode = shaderNode
        var node = createNodeFromComponent(properties)
        finalizeCreatedNode(node)
        return node
    }

    function createNodeFromComponent(properties) {
        var nodeComponent = Qt.createComponent("Node.qml")
        if(nodeComponent.status !== Component.Ready) {
            console.log(nodeComponent.errorString())
        }
        var node = nodeComponent.createObject(workspace, properties)
        return node
    }

    function finalizeCreatedNode(node) {
        node.dropReceived.connect(function(from, to) {
            var edge = createEdge(from, to)
            removeOtherEdges(edge)
            refreshOutput()
            refreshOccupation()
        })
        node.clicked.connect(function() {
            deselectAll()
            activeNode = node
            node.selected = true
        })
        nodes.push(node)
    }

    function loadTree() {
        var source = "
import ShaderNodes 1.0
StandardMaterial {
    diffuseColor: add3
    Mix {
        id: mix1
        value1: shaderBuilder.inputs[0]
        value2: Qt.vector3d(1.0, 0.0, 0.0)
    }
    Add {
        id: add3
        values: [
            shaderBuilder.inputs[0],
            mix1,
        ]
    }
}
"
        // clear the tree
        var nodesToDelete = nodes.slice()
        for(var i in nodesToDelete) {
            forceDeleteNode(nodesToDelete[i])
        }

        var object = Qt.createQmlObject(source, shaderBuilderMaterial)

        var createdNodes = []

        var rootNode = createNodesFromShaderNodeTree(object, createdNodes, 0, 0)

        for(var i in nodes) {
            var node = nodes[i]
            var originalNode = node.originalNode
            var inputNames = originalNode.inputNames()
            for(var j in inputNames) {
                var inputHandle
                for(var k in node.inputHandles) {
                    var handle = node.inputHandles[k]
                    if(handle.identifier === inputNames[j]) {
                        inputHandle = handle
                        break
                    }
                }
                if(!inputHandle) {
                    throw("Could not find input handle for " + inputNames[j])
                }

                var value = originalNode[inputHandle.identifier]
                var toExplore = []
                if(ShaderUtils.isList(value)) {
                    for(var k in value) {
                        toExplore.push(value[k])
                    }
                } else {
                    toExplore.push(value)
                }
                for(var k in toExplore) {
                    if(inputHandle.arrayBased && k > 0) {
                        inputHandle = node.addAnotherHandle(inputHandle)
                    }

                    var exploreValue = toExplore[k]
                    if(ShaderUtils.isShaderNode(exploreValue)) {
                        for(var l in nodes) {
                            var otherNode = nodes[l]
                            if(otherNode.originalNode === exploreValue) {
                                if(otherNode.outputHandles.length === 1) {
                                    // TODO support nodes with multiple or zero output values
                                    createEdge(otherNode.outputHandles[0], inputHandle)
                                }
                            }
                        }
                    } else {
                        inputHandle.value = originalNode[inputHandle.identifier]
                    }
                }
            }
        }

        root.finalNode = rootNode
        shaderBuilderMaterial.fragmentColor = finalNode.shaderNode // TODO do somewhere else
    }

    function createNodesFromShaderNodeTree(shaderNode, createdNodes, level, index) {
        if(createdNodes.indexOf(shaderNode) > -1) {
            return
        }
        createdNodes.push(shaderNode)
        console.log("Create from", shaderNode, level, index)
        var properties = {
            x: 1600 - level * 400,
            y: index * 200,
            originalNode: shaderNode
        }
        if(shaderNode.exportedTypeName) {
            properties.source = "/ShaderNodes/" + shaderNode.exportedTypeName  + ".qml"
        } else {
            properties.shaderNode = shaderNode
        }

        var node = createNodeFromComponent(properties)

        var inputNames = shaderNode.inputNames()
        var subIndex = 0
        for(var i in inputNames) {
            var name = inputNames[i]
            var value = shaderNode[inputNames[i]]

            // TODO do not create if the node is part of shaderBuilder
            // TODO do not create if this is an internal binding of the node (aka only one property needs setup)
            // TODO do not create if this is an internal object of the node (aka default light)

            var toExplore = []
            if(ShaderUtils.isList(value)) {
                for(var j in value) {
                    toExplore.push(value[j])
                }
            } else {
                toExplore.push(value)
            }

            for(var k in toExplore) {
                var exploreValue = toExplore[k]
                if(ShaderUtils.isShaderBuilderBinding(exploreValue)) {
                    continue
                }
                if(ShaderUtils.isShaderNode(exploreValue)) {
                    subIndex += 1
                    createNodesFromShaderNodeTree(exploreValue, createdNodes, level + 1, subIndex)
                }
            }
        }

        finalizeCreatedNode(node)

        return node
    }

    function indent(text) {
        return new Array(indentLevel * 4 + 1).join(" ") + text
    }

    function addOutput(text) {
        output += indent(text) + "\n"
    }

    function generateHandlesOutput(node) {
        var foundHandles = []
        var arrayHandles = {}

        for(var k in node.inputHandles) {
            var handle = node.inputHandles[k]
            if(handle.arrayBased) {
                var found = false
                for(var j in edges) {
                    var edge = edges[j]
                    var fromIdentifier = edge.from.node.identifier
                    if(edge.to === handle) {
                        if(arrayHandles[handle.identifier]) {
                            arrayHandles[handle.identifier].push(fromIdentifier)
                        } else {
                            arrayHandles[handle.identifier] = [fromIdentifier]
                        }
                        found = true
                        break
                    }
                }
                if(!found) {
                    if(handle.value != handle.defaultValue) {
                        var serializedValue = ShaderUtils.serialize(handle.value)
                        if(serializedValue) {
                            if(arrayHandles[handle.identifier]) {
                                arrayHandles[handle.identifier].push(serializedValue)
                            } else {
                                arrayHandles[handle.identifier] = [serializedValue]
                            }
                        }
                    }
                }
            } else {
                var found = false
                for(var j in edges) {
                    var edge = edges[j]
                    if(edge.to === handle) {
                        addOutput(handle.identifier + ": " + edge.from.node.identifier)
                        found = true
                        break
                    }
                }
                if(!found) {
                    if(handle.value !== handle.defaultValue) {
                        var serialized = ShaderUtils.serialize(handle.value)
                        if(serialized) {
                            addOutput(handle.identifier + ": " + serialized)
                        }
                    }
                }
            }
        }

        for(var handleName in arrayHandles) {
            var identifiers = arrayHandles[handleName]
            addOutput(handleName + ": [")
            indentLevel += 1
            for(var i in identifiers) {
                addOutput(identifiers[i] + ",")
            }
            indentLevel -= 1
            addOutput("]")
        }
    }

    function refreshOutput() {
        if(!finalNode) {
            console.warn("ERROR: Cannot generate tree.")
            console.warn("ERROR: Final node not set!")
            return false
        }

        output = "\n"
        indentLevel = 0

        addOutput("import ShaderNodes 1.0")

        // Generate ids
        var counter = 1
        for(var i in nodes) {
            var node = nodes[i]
            if(node.virtualized) {
                continue
            }
            node.identifier = node.exportedTypeName.toLowerCase() + counter
            counter += 1
        }

        addOutput(finalNode.exportedTypeName + " {")
        indentLevel += 1

        //        addOutput("property ShaderBuilder shaderBuilder: parent.shaderBuilder")
        generateHandlesOutput(finalNode)

        for(var i in nodes) {
            var node = nodes[i]
            if(node === finalNode) {
                continue
            }
            if(node.virtualized) {
                continue
            }
            addOutput(node.exportedTypeName + " {")
            indentLevel += 1
            addOutput("id: " + node.identifier)
            generateHandlesOutput(node)

            indentLevel -= 1
            addOutput("}")
        }

        indentLevel -= 1
        addOutput("}")

        //        var material = Qt.createQmlObject(output, root, "GeneratedMaterial")
        //        material.shaderBuilder = shaderBuilder
//        shaderBuilderMaterial.fragmentColor = finalNode.shaderNode // TODO do somewhere else

        return true
    }
    
    Flickable {
        id: workspaceFlickable
        
        anchors.fill: parent
        
        contentHeight: workspace.height
        contentWidth: workspace.width
        boundsBehavior: Flickable.DragAndOvershootBounds
        
        ScrollBar.vertical: ScrollBar {}
        ScrollBar.horizontal: ScrollBar {}
        
        Item {
            id: workspace
            
            width: 3840
            height: 2160

            MouseArea {
                anchors.fill: parent
                onClicked: deselectAll()
            }
        }
    }

    Popup {
        id: popup
        x: root.width / 2 - width / 2
        y: root.height / 2 - height / 2
        width: 320
        height: 240
        modal: false
        focus: true
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

        function show(message) {
            popupLabel.text = message
            popup.open()
        }

        Label {
            id: popupLabel
            anchors.centerIn: parent
            text: "Error!"
        }
    }

    Keys.onDeletePressed: {
        deleteSelected()
    }
}
