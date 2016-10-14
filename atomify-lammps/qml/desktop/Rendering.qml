import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Controls 1.5 as QQC1
import Atomify 1.0
import "../visualization"
Pane {
    id: root
    property AtomifyVisualizer visualizer

    onVisualizerChanged: {
        light1Strength.value = visualizer.light1.strength
        light1Attenuation.value = visualizer.light1.attenuation
        light2Strength.value = visualizer.light2.strength
        light2Attenuation.value = visualizer.light2.attenuation
        periodicXSlider.value = visualizer.periodicImages.numberOfCopiesX
        periodicYSlider.value = visualizer.periodicImages.numberOfCopiesY
        periodicZSlider.value = visualizer.periodicImages.numberOfCopiesZ
    }

    Column {
        anchors {
            fill: parent
            margins: 10
        }
        spacing: 10


        GroupBox {
            anchors {
                left: parent.left
                right: parent.right
            }

            title: "Interface"

            CheckBox {
                id: guidesCheckBox
                text: "Show guides"
                checked: visualizer.guidesVisible
                Binding {
                    target: visualizer
                    property: "guidesVisible"
                    value: guidesCheckBox.checked
                }
            }

        }

        GroupBox {
            width: parent.width
            title: "Light 1"
            Column {
                width: parent.width
                Row {
                    width: parent.width
                    Label {
                        width: parent.width*0.4
                        text: "Strength"
                    }
                    QQC1.Slider {
                        id: light1Strength
                        width: parent.width*0.6
                        minimumValue: 0
                        maximumValue: 1
                        onValueChanged: {
                            visualizer.light1.strength = value
                        }
                    }
                }

                Row {
                    width: parent.width
                    Label {
                        width: parent.width*0.4
                        text: "Attenuation"
                    }
                    QQC1.Slider {
                        id: light1Attenuation
                        width: parent.width*0.6
                        minimumValue: 0
                        maximumValue: 5
                        onValueChanged: visualizer.light1.attenuation = value
                    }
                }
            }
        }

        GroupBox {
            width: parent.width
            title: "Light 2"
            Column {
                width: parent.width
                Row {
                    width: parent.width
                    Label {
                        width: parent.width*0.4
                        text: "Strength"
                    }
                    QQC1.Slider {
                        id: light2Strength
                        width: parent.width*0.6
                        minimumValue: 0
                        maximumValue: 1
                        onValueChanged: {
                            visualizer.light2.strength = value
                        }
                    }
                }

                Row {
                    width: parent.width
                    Label {
                        width: parent.width*0.4
                        text: "Attenuation"
                    }
                    QQC1.Slider {
                        id: light2Attenuation
                        width: parent.width*0.6
                        minimumValue: 0
                        maximumValue: 5
                        onValueChanged: visualizer.light2.attenuation = value
                    }
                }
            }
        }

        GroupBox {
            width: parent.width
            title: "Periodic images"
            Column {
                width: parent.width
                Row {
                    width: parent.width
                    height: periodicXSlider.height
                    Label {
                        width: parent.width*0.4
                        text: "X = "+periodicXSlider.value
                    }
                    QQC1.Slider {
                        id: periodicXSlider
                        width: parent.width*0.6
                        minimumValue: 1
                        maximumValue: 5.0
                        value: 1
                        onValueChanged: visualizer ? visualizer.periodicImages.numberOfCopiesX = value : ""
                        stepSize: 1
                    }
                }

                Row {
                    width: parent.width
                    height: periodicYSlider.height
                    Label {
                        width: parent.width*0.4
                        text: "Y = "+periodicYSlider.value
                    }
                    QQC1.Slider {
                        id: periodicYSlider
                        width: parent.width*0.6
                        minimumValue: 1
                        maximumValue: 5.0
                        onValueChanged: visualizer ? visualizer.periodicImages.numberOfCopiesY = value : ""
                        stepSize: 1
                    }
                }

                Row {
                    width: parent.width
                    height: periodicZSlider.height
                    Label {
                        width: parent.width*0.4
                        text: "Z = "+periodicZSlider.value
                    }
                    QQC1.Slider {
                        id: periodicZSlider
                        width: parent.width*0.6
                        minimumValue: 1
                        maximumValue: 5.0
                        onValueChanged: visualizer ? visualizer.periodicImages.numberOfCopiesZ = value : ""
                        stepSize: 1
                    }
                }
            }
        }

        GroupBox {
            width: parent.width
            title: "Rendering quality"

            Binding {
                target: visualizer
                property: "renderQuality"
                value: buttonGroup.checkedButton.quality
            }
            ButtonGroup {
                id: buttonGroup
                buttons: qualityButtonRow.children
            }

            Row {
                id: qualityButtonRow
                width: parent.width
                RadioButton {
                    id: renderingQualityLow
                    property string quality: "low"
                    checked: visualizer.renderQuality === quality
                    focusPolicy: Qt.NoFocus
                    text: "Low"
                }
                RadioButton {
                    id: renderingQualityMedium
                    property string quality: "medium"
                    checked: visualizer.renderQuality === quality
                    focusPolicy: Qt.NoFocus
                    text: "Medium"
                }
                RadioButton {
                    id: renderingQualityHigh
                    property string quality: "high"
                    checked: visualizer.renderQuality === quality
                    focusPolicy: Qt.NoFocus
                    text: "High"
                }
            }
        }

        GroupBox {
            width: parent.width
            title: "Rendering mode"

            ComboBox {
                width: parent.width
                enabled: visualizer.renderQuality !== "low"
                focusPolicy: Qt.NoFocus
                model: ["Normal", "SEM", "Blurred SEM", "position", "color", "normal", "depth"]
                currentIndex: 0
                onCurrentTextChanged: {
                    visualizer.finalShaderBuilder.selectOutput(currentText)
                }
            }
        }
    }
}
