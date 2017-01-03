import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import Qt.labs.settings 1.0

ApplicationWindow {
    id: root
    visible: true
    width: 1920
    height: 1080
    title: qsTr("ShaderNodes")

    Settings {
        id: settings
        property alias windowWidth: root.width
        property alias windowHeight: root.height
    }

    ColumnLayout {
        id: leftColumn
        anchors {
            left: parent.left
            top: parent.top
            bottom: parent.bottom
        }
        width: parent.width * 0.10

        Button {
            id: loadButton
            text: "Load"
            onClicked: mainEditor.loadTree()
        }

        Button {
            id: exportButton
            text: "Export"
            onClicked: mainEditor.logOutput()
        }

        ListView {
            id: shaderList
            Layout.fillHeight: true
            Layout.fillWidth: true
            model: ListModel {
                ListElement {name: "Add"}
                ListElement {name: "AmbientOcclusion"}
                ListElement {name: "Bump"}
                ListElement {name: "Clamp"}
                ListElement {name: "CombineRgb"}
                ListElement {name: "Decolorize"}
                ListElement {name: "Displacement"}
    //            ListElement {name: "Function"}
    //            ListElement {name: "Geometry"}
                ListElement {name: "ImageTexture"}
                ListElement {name: "Light"}
                ListElement {name: "Mix"}
                ListElement {name: "Multiply"}
                ListElement {name: "Noise"}
                ListElement {name: "NormalMap"}
                ListElement {name: "Simplex"}
                ListElement {name: "Sine"}
    //            ListElement {name: "Split"}
                ListElement {name: "StandardMaterial"}
                ListElement {name: "Subtract"}
                ListElement {name: "Sum"}
            }

            delegate: ItemDelegate {
                text: name
                onClicked: {
                    mainEditor.createNode("qrc:/ShaderNodes/" + name + ".qml", {x: 200, y: 200})
                }
            }
        }
    }

    Editor {
        id: mainEditor

        anchors {
            left: leftColumn.right
            top: parent.top
            bottom: parent.bottom
        }

        width: parent.width * 0.6

        shaderBuilderMaterial: previewScene.material
    }

    PropertiesPane {
        id: propertiesPane

        anchors {
            top: parent.top
            bottom: parent.verticalCenter
            right: parent.right
            left: mainEditor.right
        }

        editor: mainEditor
    }

//    Pane {
//        id: shader
//        anchors {
//            top: parent.top
//            left: editor.right
//            bottom: parent.verticalCenter
//            right: parent.right
//        }
//        clip: true

//        Flickable {
//            anchors {
//                margins: 16
//                fill: parent
//            }

//            contentWidth: shaderText.width
//            contentHeight: shaderText.height

//            ScrollBar.vertical: ScrollBar {}
//            ScrollBar.horizontal: ScrollBar {}

//            Label {
//                id: shaderText
////                selectByKeyboard: true
////                selectByMouse: true
////                readOnly: true
//                text: editor.output //+ "\n" + editor.shaderBuilder.finalShader
//            }
//        }
//    }

    PreviewScene {
        id: previewScene

        anchors {
            right: parent.right
            top: parent.verticalCenter
            left: mainEditor.right
            bottom: parent.bottom
        }
    }

    Rectangle {
        id: dropRectangle
        anchors.fill: previewScene

        opacity: 0
        color: "grey"

        Label {
            text: "Drop 3D models here"
            anchors.centerIn: parent
        }

        DropArea {
            id: dropArea
            anchors.fill: parent
            onEntered: {
                drag.accept(Qt.CopyAction)
            }
            onDropped: {
                if(drop.hasUrls) {
                    var fileUrl = drop.urls[0]
                    previewScene.source = fileUrl
                }
            }
        }

        states: [
            State {
                when: dropArea.containsDrag
                PropertyChanges {
                    target: dropRectangle
                    opacity: 1.0
                }
            }
        ]
    }
}
