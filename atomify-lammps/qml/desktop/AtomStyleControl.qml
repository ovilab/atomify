import QtQuick 2.4
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import AtomStyle 1.0
import QtQuick.Dialogs 1.0

Item {
    property AtomStyle atomStyle
    height: listview.height+button.height
    width: parent.width

    ListView {
        id: listview
        height: count*26
        model: atomStyle.model
        delegate: Row {
            spacing: 5

            CheckBox {
                y: 4
                text: 'Atom type ' + (index+1) + ":"
                checked: model.modelData.visible
                onCheckedChanged: {
                    atomStyle.setModelData(index, "visible", checked)
                }
            }

            ComboBox {
                id: atomDatathing
                model: atomDataModel
                onCurrentIndexChanged: {
                    if(currentIndex > 0) {
                        scale.value = model.get(currentIndex).radius/100 // We want angstroms, these are in pm
                        atomStyle.setModelData(index, "color", model.get(currentIndex).color)
                    }
                }
            }

            Text {
                y: 4
                text: "Color:"
            }

            Rectangle {
                id: colorRectangle
                y: 2
                height: 20
                width: 20
                color: model.modelData.color
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        colorDialog.visible = true
                        colorDialog.clickedIndex = index
                        colorDialog.color = model.modelData.color
                    }
                }
            }

            SpinBox {
                id: scale
                minimumValue: 0
                maximumValue: 3
                decimals: 1
                stepSize: 0.1
                value: model.modelData.scale
                prefix: "Size: "
                onValueChanged: atomStyle.setModelData(index, "scale", value)
            }

            Image {
                height: 20
                width: 20
                source: "/images/close.png"
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
        property int clickedIndex: -1
        onColorChanged: {
            if(clickedIndex >= 0) {
                atomStyle.setModelData(clickedIndex, "color", colorDialog.color)
            }
        }
        onAccepted: {
            clickedIndex = -1
        }
        onRejected: {
            clickedIndex = -1
        }
    }

    Button {
        id: button
        anchors.top: listview.bottom
        text: "Add"
        onClicked: {
            atomStyle.add()
        }
    }

    ListModel {
        id: atomDataModel
        // Colors from JMOL: http://jmol.sourceforge.net/jscolors/
        // Atomic radii from wikipedia (van der waals radius)
        ListElement { text: "Custom"; radius: 100; color: "#FFFFFF" }
        ListElement { text: "1: Hydrogen (H)"; radius: 120; color: "#FFFFFF" }
        ListElement { text: "2: Helium (He)"; radius: 140; color: "#D9FFFF" }
        ListElement { text: "3: Lithium (Li)"; radius: 182; color: "#CC80FF" }
        ListElement { text: "4: Beryllium (Be)"; radius: 153; color: "#C2FF00" }
        ListElement { text: "5: Boron (B)"; radius: 192; color: "#FFB5B5" }
        ListElement { text: "6: Carbon (C)"; radius: 170; color: "#909090" }
        ListElement { text: "7: Nitrogen (N)"; radius: 155; color: "#3050F8" }
        ListElement { text: "8: Oxygen (O)"; radius: 152; color: "#FF0D0D" }
        ListElement { text: "9: Fluorine (F)"; radius: 135; color: "#90E050" }
        ListElement { text: "10: Neon (Ne)"; radius: 154; color: "#3050F8" }
        ListElement { text: "11: Sodium (Na)"; radius: 227; color: "#AB5CF2" }
        ListElement { text: "12: Magnesium (Mg)"; radius: 173; color: "#8AFF00" }
        ListElement { text: "13: Aluminium (Al)"; radius: 184; color: "#BFA6A6" }
        ListElement { text: "14: Silicon (Si)"; radius: 227; color: "#F0C8A0" }
        ListElement { text: "15: Phosphorus (P)"; radius: 180; color: "#FF8000" }
        ListElement { text: "16: Sulfur (S)"; radius: 180; color: "#FFFF30" }
        ListElement { text: "17: Chlorine (Cl)"; radius: 175; color: "#1FF01F" }
        ListElement { text: "18: Argon (Ar)"; radius: 188; color: "#80D1E3" }
        ListElement { text: "19: Potassium (K)"; radius: 275; color: "#8F40D4" }
        ListElement { text: "20: Calcium (Ca)"; radius: 231; color: "#3DFF00" }
    }
}
