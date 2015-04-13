import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import SimVis 1.0
import MySimulator 1.0

Rectangle {
    id: renderControlRoot
    width: controls.width + 10
    height: controls.height + 10
    property SimplexBump simplexBump
    property Spheres spheres
    color: "#ffffff"
    opacity: 0.9

    onVisibleChanged: {
        focus = visible
    }

    ColumnLayout {
        id: controls
        anchors.centerIn: parent

        GroupBox {
            id: spheresGroup
            width: parent.width
            title: qsTr("Spheres")

            RowLayout {

                Label {
                    id: sizeLabel
                    text: "Size"
                }

                Slider {
                    id: size
                    Layout.fillWidth: true
                    minimumValue: 0.01
                    maximumValue: 0.25
                    value: spheres.scale
                    onValueChanged: {
                        if(spheres != null) {
                            spheres.scale = value
                            sizeLabel.text = "Size ("+spheres.scale.toFixed(2)+")"
                        }
                    }
                }
            }
        }

        GroupBox {
            id: bump
            width: parent.width
            title: qsTr("Simplex bump")

            GridLayout {
                columns: 2
                CheckBox {
                    id: bumpEnabled
                    Layout.columnSpan: 2
                    text: "Enabled"
                    checked: simplexBump.enabled
                    onCheckedChanged: {
                        if(simplexBump != null) {
                            simplexBump.enabled = checked
                        }
                    }
                }

                Label {
                    id: strengthLabel
                    text: qsTr("Strength")
                }

                Slider {
                    id: bumpIntensity
                    Layout.fillWidth: true
                    minimumValue: 0.0
                    maximumValue: 0.3
                    value: simplexBump.intensity
                    onValueChanged: simplexBump.intensity = value
                }


                Label {
                    id: scaleLabel
                    width: strengthLabel.width
                    text: qsTr("Scale")
                }

                Slider {
                    id: bumpScale
                    Layout.fillWidth: true
                    minimumValue: 0.0
                    maximumValue: 10.0
                    value: simplexBump.scale
                    onValueChanged: simplexBump.scale = value
                }
            }
        }

        Button {
            id: okButton
            Layout.columnSpan: 2
            anchors.horizontalCenter: parent.horizontalCenter
            x: parent.width*0.5 - width*0.5
            text: qsTr("OK")
            onClicked: renderControlRoot.visible = false
        }
    }

    Keys.onPressed: {
        if(event.key === Qt.Key_Escape) {
            visible = false
        }
    }
}

