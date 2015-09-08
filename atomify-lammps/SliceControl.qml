import QtQuick 2.3
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import MySimulator 1.0

Item {
    id: sliceRoot
    property MySimulator simulator

    ColumnLayout {
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
                value: simulator ? simulator.sliceDistance : 1
                onValueChanged: {
                    simulator.sliceDistance = value
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
                text: simulator ? simulator.sliceNormal.x : 0
                Layout.preferredWidth: 200
                inputMethodHints: Qt.ImhFormattedNumbersOnly

                onTextChanged: {
                    var x = parseFloat(normalXText.text)
                    var y = parseFloat(simulator.sliceNormal.y)
                    var z = parseFloat(simulator.sliceNormal.z)
                    if(text.length > 0) {
                        simulator.sliceNormal = Qt.vector3d(x,y,z)
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
                text: simulator ? simulator.sliceNormal.y : 0
                Layout.preferredWidth: 200
                inputMethodHints: Qt.ImhFormattedNumbersOnly
                onTextChanged: {
                    var x = parseFloat(simulator.sliceNormal.x)
                    var y = parseFloat(normalYText.text)
                    var z = parseFloat(simulator.sliceNormal.z)

                    if(text.length > 0) {
                        simulator.sliceNormal = Qt.vector3d(x,y,z)
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
                text: simulator ? simulator.sliceNormal.z : 0
                Layout.preferredWidth: 200
                inputMethodHints: Qt.ImhFormattedNumbersOnly

                onTextChanged: {
                    var x = parseFloat(simulator.sliceNormal.x)
                    var y = parseFloat(simulator.sliceNormal.y)
                    var z = parseFloat(normalZText.text)

                    if(text.length > 0) {
                        simulator.sliceNormal = Qt.vector3d(x,y,z)
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
                value: simulator ? simulator.sliceWidth : 1
                onValueChanged: {
                    simulator.sliceWidth = value
                }
            }
        }
    }
}

