import QtQuick 2.4
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import MySimulator 1.0
import SimVis 1.0

Item {
    id: sliceRoot
    property Slice slice
    width: parent.width
    height: columnLayout.height
    ColumnLayout {
        id: columnLayout
        spacing: 10

        Row {
            spacing: 2
            Label {
                text: "Distance: "
            }

            SpinBox {
                id: distance
                decimals: 1
                minimumValue: 0
                maximumValue: 200
                stepSize: 1
                value: slice ? slice.distance : 1
                onValueChanged: {
                    slice.distance = value
                }
            }
        }

        Row {
            spacing: 2
            Label {
                text: "Normal (X): "
            }

            TextField {
                id: normalXText
                text: slice ? slice.normal.x : 0
                Layout.preferredWidth: 200
                inputMethodHints: Qt.ImhFormattedNumbersOnly

                onTextChanged: {
                    var x = parseFloat(normalXText.text)
                    var y = parseFloat(slice.normal.y)
                    var z = parseFloat(slice.normal.z)
                    if(text.length > 0) {
                        slice.normal = Qt.vector3d(x,y,z)
                    }
                }
            }
        }

        Row {
            spacing: 2
            Label {
                text: "Normal (Y): "
            }

            TextField {
                id: normalYText
                text: slice ? slice.normal.y : 0
                Layout.preferredWidth: 200
                inputMethodHints: Qt.ImhFormattedNumbersOnly
                onTextChanged: {
                    var x = parseFloat(slice.normal.x)
                    var y = parseFloat(normalYText.text)
                    var z = parseFloat(slice.normal.z)

                    if(text.length > 0) {
                        slice.normal = Qt.vector3d(x,y,z)
                    }
                }
            }
        }

        Row {
            spacing: 2
            Label {
                text: "Normal (Z): "
            }

            TextField {
                id: normalZText
                text: slice ? slice.normal.z : 0
                Layout.preferredWidth: 200
                inputMethodHints: Qt.ImhFormattedNumbersOnly

                onTextChanged: {
                    var x = parseFloat(slice.normal.x)
                    var y = parseFloat(slice.normal.y)
                    var z = parseFloat(normalZText.text)

                    if(text.length > 0) {
                        slice.normal = Qt.vector3d(x,y,z)
                    }
                }
            }
        }

        Row {
            spacing: 2
            Label {
                text: "Slice width: "
            }

            SpinBox {
                id: sliceWidth
                decimals: 1
                minimumValue: 0
                maximumValue: 200
                stepSize: 1
                value: slice ? slice.width : 1
                onValueChanged: {
                    slice.width = value
                }
            }
        }
    }
}

