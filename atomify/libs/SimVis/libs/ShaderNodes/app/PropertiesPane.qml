import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import Qt.labs.settings 1.0
import QtGraphicalEffects 1.0
import QtQuick.Controls 1.0 as QtControls
import QtQuick.Dialogs 1.2

import ShaderNodes 1.0

import "widgets"

Pane {
    id: root

    // TODO add possibility to change handle value type manually

    property Editor editor
    readonly property bool valid: (editor && editor.activeNode)
    
    Column {
        anchors {
            left: parent.left
            right: parent.right
        }
        
        Label {
            text: valid ? editor.activeNode.title : ""
        }
        
        Repeater {
            model: valid ? editor.activeNode.inputHandles : undefined
            
            Item {
                property Handle handle: modelData
                anchors {
                    left: parent.left
                    right: parent.right
                }
                height: loader.height

                Component {
                    id: floatEditComponent
                    FloatEditor {
                        handle: modelData
                    }
                }

                Component {
                    id: urlComponent
                    Item {
                        anchors {
                            left: parent.left
                            right: parent.right
                        }
                        height: textField.height

                        TextField {
                            id: textField
                            anchors {
                                left: parent.left
                                right: browseButton.left
                            }

                            text: handle.value
                            onEditingFinished: {
                                handle.value = Qt.resolvedUrl(text)
                            }
                        }
                        Button {
                            id: browseButton
                            anchors {
                                right: parent.right
                            }

                            text: "..."
                            onClicked: fileDialog.open()
                        }

                        FileDialog {
                            id: fileDialog
                            onAccepted: {
                                textField.text = fileUrl
                                handle.value = fileUrl
                            }
                        }
                    }
                }

                Component {
                    id: vector3DComponent
                    Vector3DEditor {
                        handle: modelData
                    }
                }
                
                Component {
                    id: occupiedComponent
                    Label {
                        text: handle.name + ": node"
                    }
                }

                Component {
                    id: unknownComponent
                    Label {
                        text: handle.name + ": default"
                    }
                }

                Component {
                    id: colorComponent
                    ColorEditor {
                        id: colorPicker
                        handle: modelData
                    }
                }
                
                Loader {
                    id: loader
                    anchors {
                        left: parent.left
                        right: parent.right
                    }
                    
                    sourceComponent: {
                        if(handle.occupied) {
                            return occupiedComponent
                        }
                        switch(ShaderUtils.variantType(handle.value)) {
                        case ShaderUtils.Url:
                            return urlComponent
                        case ShaderUtils.Int:
                        case ShaderUtils.Float:
                            return floatEditComponent
                        case ShaderUtils.Color:
                        case ShaderUtils.String:
                            return colorComponent
                        case ShaderUtils.Vector3D:
                            return vector3DComponent
                        default:
                            return unknownComponent
                        }
                    }
                }
            }
        }
    }
}
