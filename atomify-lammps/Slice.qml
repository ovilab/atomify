import QtQuick 2.3
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import MySimulator 1.0

Item {
    id: sliceRoot
    property MySimulator simulator
    onSimulatorChanged: {
        distance.value = simulator.sliceDistance
        normalXText.text = simulator.sliceNormal.x
        normalYText.text = simulator.sliceNormal.y
        normalZText.text = simulator.sliceNormal.z
        sliceWidth.value = simulator.sliceWidth
    }

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
                text: "0"
                Layout.preferredWidth: 200
                inputMethodHints: Qt.ImhFormattedNumbersOnly

                onTextChanged: {
                    if(simulator != undefined) {
                        var x = parseFloat(normalXText.text)
                        var y = parseFloat(simulator.sliceNormal.y)
                        var z = parseFloat(simulator.sliceNormal.z)
                        if(text.length > 0) {
                            simulator.sliceNormal = Qt.vector3d(x,y,z)
                        }
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
                text: "0"
                Layout.preferredWidth: 200
                inputMethodHints: Qt.ImhFormattedNumbersOnly
                onTextChanged: {
                    if(simulator != undefined) {
                        var x = parseFloat(simulator.sliceNormal.x)
                        var y = parseFloat(normalYText.text)
                        var z = parseFloat(simulator.sliceNormal.z)

                        if(text.length > 0) {
                            simulator.sliceNormal = Qt.vector3d(x,y,z)
                        }
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
                text: "0"
                Layout.preferredWidth: 200
                inputMethodHints: Qt.ImhFormattedNumbersOnly

                onTextChanged: {
                    if(simulator != undefined) {
                        var x = parseFloat(simulator.sliceNormal.x)
                        var y = parseFloat(simulator.sliceNormal.y)
                        var z = parseFloat(normalZText.text)

                        if(text.length > 0) {
                            simulator.sliceNormal = Qt.vector3d(x,y,z)
                        }
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
                onValueChanged: {
                    simulator.sliceWidth = value
                }
            }
        }
    }
}

