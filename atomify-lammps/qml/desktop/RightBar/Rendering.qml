import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Controls 1.5 as QQC1
import Atomify 1.0
import Qt.labs.settings 1.0
import "../../visualization"
Pane {
    id: root
    property AtomifyVisualizer visualizer

    Settings {
        property alias showGuides: guidesCheckBox.checked
        property alias showOutline: outlineCheckBox.checked
    }

    Flickable {
        anchors.fill: parent
        flickableDirection: Flickable.VerticalFlick
        contentHeight: column.height
        ScrollBar.vertical: ScrollBar {}

        Column {
            id: column
            anchors {
                left: parent.left
                right: parent.right
                margins: 10
            }

            spacing: 10


            GroupBox {
                anchors {
                    left: parent.left
                    right: parent.right
                }

                title: "Interface"

                Column {
                    anchors {
                        left: parent.left
                        right: parent.right
                    }

                    CheckBox {
                        id: guidesCheckBox
                        text: "Show guides"
                        checked: visualizer.guidesVisible
                        focusPolicy: Qt.NoFocus
                        Binding {
                            target: visualizer
                            property: "guidesVisible"
                            value: guidesCheckBox.checked
                        }
                    }

                    Shortcut {
                        sequence: "G"
                        onActivated: visualizer.guidesVisible = !visualizer.guidesVisible
                    }

                    CheckBox {
                        id: outlineCheckBox
                        text: "Show outline"
                        checked: visualizer.systemBoxVisible
                        focusPolicy: Qt.NoFocus
                        Binding {
                            target: visualizer
                            property: "systemBoxVisible"
                            value: outlineCheckBox.checked
                        }
                    }

                    Shortcut {
                        sequence: "M"
                        onActivated: visualizer.systemBoxVisible = !visualizer.systemBoxVisible
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
                            value: visualizer.light1.strength

                            Binding {
                                target: visualizer.light1
                                property: "strength"
                                value: light1Strength.value
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
                            maximumValue: 50
                            value: visualizer.light1.attenuation

                            Binding {
                                target: visualizer.light1
                                property: "attenuation"
                                value: light1Attenuation.value
                            }
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
                            value: visualizer.light2.strength

                            Binding {
                                target: visualizer.light2
                                property: "strength"
                                value: light2Strength.value
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
                            maximumValue: 50
                            value: visualizer.light2.attenuation

                            Binding {
                                target: visualizer.light2
                                property: "attenuation"
                                value: light2Attenuation.value
                            }
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
                            text: "X: "+periodicXSlider.value
                        }
                        QQC1.Slider {
                            id: periodicXSlider
                            width: parent.width*0.6
                            minimumValue: 1
                            maximumValue: 5.0
                            stepSize: 1
                            value: visualizer.periodicImages.numberOfCopiesX
                            Binding {
                                target: visualizer.periodicImages
                                property: "numberOfCopiesX"
                                value: periodicXSlider.value
                            }
                        }
                    }

                    Row {
                        width: parent.width
                        height: periodicYSlider.height
                        Label {
                            width: parent.width*0.4
                            text: "Y: "+periodicYSlider.value
                        }
                        QQC1.Slider {
                            id: periodicYSlider
                            width: parent.width*0.6
                            minimumValue: 1
                            maximumValue: 5.0
                            stepSize: 1
                            value: visualizer.periodicImages.numberOfCopiesY
                            Binding {
                                target: visualizer.periodicImages
                                property: "numberOfCopiesY"
                                value: periodicYSlider.value
                            }
                        }
                    }

                    Row {
                        width: parent.width
                        height: periodicZSlider.height
                        Label {
                            width: parent.width*0.4
                            text: "Z: "+periodicZSlider.value
                        }
                        QQC1.Slider {
                            id: periodicZSlider
                            width: parent.width*0.6
                            minimumValue: 1
                            maximumValue: 5.0
                            stepSize: 1
                            value: visualizer.periodicImages.numberOfCopiesZ
                            Binding {
                                target: visualizer.periodicImages
                                property: "numberOfCopiesZ"
                                value: periodicZSlider.value
                            }
                        }
                    }
                }
            }

            GroupBox {
                width: parent.width
                title: "Rendering mode"
                Column {
                    width: parent.width

                    RadioButton {
                            checked: true
                            text: qsTr("Ball and stick")
                            onCheckedChanged: {
                                if(checked) {
                                    visualizer.simulator.system.atoms.renderingMode = "Ball and stick"
                                    sphereScaleSlider.value = 0.0
                                    bondScaleSlider.value = 1.0
                                }
                            }
                        }
                        RadioButton {
                            text: qsTr("Stick")
                            onCheckedChanged: {
                                if(checked) {
                                    visualizer.simulator.system.atoms.renderingMode = "Stick"
                                    sphereScaleSlider.value = 0.0
                                    bondScaleSlider.value = 0.3
                                }
                            }
                        }
                        RadioButton {
                            text: qsTr("Wireframe")
                            onCheckedChanged: {
                                if(checked) {
                                    visualizer.simulator.system.atoms.renderingMode = "Wireframe"
                                    sphereScaleSlider.value = 0.1
                                    bondScaleSlider.value = 1.5
                                }
                            }
                        }

                    Row {
                        width: parent.width
                        height: sphereScaleSlider.height
                        Label {
                            width: parent.width*0.4
                            text: "Sphere size: "
                        }
                        QQC1.Slider {
                            id: sphereScaleSlider
                            property real expValue: Math.exp(value)
                            enabled: renderingMode.currentText==="Ball and stick"
                            width: parent.width*0.6
                            minimumValue: -2.0
                            maximumValue: 4.0
                            stepSize: 0.1
                            value: Math.log(visualizer.simulator.system.atoms.sphereScale)
                            Binding {
                                target: visualizer.simulator.system.atoms
                                property: "sphereScale"
                                value: sphereScaleSlider.expValue
                            }
                        }
                    }

                    Row {
                        width: parent.width
                        height: bondScaleSlider.height
                        Label {
                            width: parent.width*0.4
                            text: "Bond size: "
                        }
                        QQC1.Slider {
                            id: bondScaleSlider
                            width: parent.width*0.6
                            minimumValue: 0.1
                            maximumValue: 2.0
                            stepSize: 0.1
                            value: visualizer.simulator.system.atoms.bondScale
                            Binding {
                                target: visualizer.simulator.system.atoms
                                property: "bondScale"
                                value: bondScaleSlider.value
                            }
                        }
                    }
                }
            }
        }
    }
}
