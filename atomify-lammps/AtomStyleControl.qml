import QtQuick 2.3
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import AtomStyle 1.0
import QtQuick.Dialogs 1.0

Item {
    property AtomStyle atomStyle
    //    onAtomStyleChanged: {
    //        listview.model = atomStyle.model
    //    }

    Item {
        anchors.fill: parent

        ListView {
            id: listview
            anchors.top: button.bottom
            // anchors.fill: parent
            height: count*40
            width: 300
            model: atomStyle.model
            delegate: Row {
                width: 180
                height: 30
                spacing: 5
                Text {
                    y: 5
                    text: 'Atom type ' + (index+1) + ":"
                }

                SpinBox {
                    minimumValue: 0
                    maximumValue: 3
                    decimals: 1
                    stepSize: 0.1
                    value: model.modelData.scale
                    prefix: "Scale: "
                    onValueChanged: atomStyle.setModelData(index, "scale", value)
                }


                Rectangle {
                    height: 20
                    width: 20
                    color: model.modelData.color
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            colorDialog.color = model.modelData.color
                            colorDialog.clickedIndex = index
                            colorDialog.visible = true
                        }
                    }
                }

                Image {
                    height: 20
                    width: 20
                    source: "images/close.png"
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            atomStyle.remove(index)
                        }
                    }
                }
            }
        }

        ColorDialog {
            id: colorDialog
            property int clickedIndex
            onColorChanged: {
                atomStyle.setModelData(clickedIndex, "color", colorDialog.color)
            }
        }

        Button {
            id: button
            text: "Add"
            onClicked: {
                atomStyle.add()
            }
        }
    }


}
