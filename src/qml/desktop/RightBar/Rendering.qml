import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Controls 1.5 as QQC1
import QtQuick.Dialogs 1.2
import Atomify 1.0
import Qt.labs.settings 1.0
import "../../visualization"
import "../../events"
Pane {
    id: root
    property AtomifyVisualizer visualizer
    onVisualizerChanged: {
        if(visualizer) {
            visualizer.simulator.command.connect(function(cmd) {
                var words = cmd.split(" ")
                if(words[0]==="scale") {
                    var type = words[1]
                    var value = parseFloat(words[2])
                    if(type==="spheres" || type==="sphere") {
                        sphereScaleSlider.value = Math.log(value)
                    } else if(type==="global") {
                        globalScaleSlider.value = value
                    } else if(type==="bonds" || type==="bond")  {
                        bondScaleSlider.value = value
                    }
                }
            })
        }
    }

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

            SliceControl {
                id: sliceControl
                width: parent.width
                slice: visualizer.sliceModifier
            }
            PeriodicImagesControl {
                id: periodicCopiesControl
                width: parent.width
                periodicImages: visualizer.periodicImages
            }

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
                        text: "Show axis guides"
                        checked: visualizer.guidesVisible
                        focusPolicy: Qt.NoFocus
                        hoverEnabled: true
                        ToolTip.visible: hovered
                        ToolTip.delay: 1000
                        ToolTip.text: "Show/hide coordinate axis guides ("+EventCenter.nativeText("visualizer.showguides")+")"
                        Binding {
                            target: visualizer
                            property: "guidesVisible"
                            value: guidesCheckBox.checked
                        }
                    }

                    CheckBox {
                        id: outlineCheckBox
                        text: "Show box outline"
                        checked: visualizer.systemBoxVisible
                        focusPolicy: Qt.NoFocus
                        ToolTip.visible: hovered
                        ToolTip.delay: 1000
                        ToolTip.text: "Show/hide system box defining simulation region ("+EventCenter.nativeText("visualizer.showoutline")+")"
                        Binding {
                            target: visualizer
                            property: "systemBoxVisible"
                            value: outlineCheckBox.checked
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
                        id: ballAndStick
                        checked: true
                        text: qsTr("Ball and stick")
                        focusPolicy: Qt.NoFocus
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
                        focusPolicy: Qt.NoFocus
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
                        focusPolicy: Qt.NoFocus
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
                            enabled: ballAndStick.checked
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

                    Row {
                        width: parent.width
                        height: globalScaleSlider.height
                        Label {
                            width: parent.width*0.4
                            text: "Global scale: "
                        }
                        QQC1.Slider {
                            id: globalScaleSlider
                            width: parent.width*0.6
                            minimumValue: 0.5
                            maximumValue: 3.0
                            stepSize: 0.1
                            value: visualizer.simulator.system.atoms.globalScale
                            Binding {
                                target: visualizer.simulator.system.atoms
                                property: "globalScale"
                                value: globalScaleSlider.value
                            }
                        }
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
                            maximumValue: 5
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
                            maximumValue: 20
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
                height: 75
                title: "Background color"
                Rectangle {
                    id: backgroundColor
                    anchors.fill: parent
                    color: visualizer.backgroundColor

                    Binding {
                        target: visualizer
                        property: "backgroundColor"
                        value: color
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            colorDialog.open()
                        }
                    }
                }
            }
        }
    }

    ColorDialog {
        id: colorDialog
        title: "Please choose a color"
        color: visualizer.backgroundColor
        currentColor: visualizer.backgroundColor
        onCurrentColorChanged: visualizer.backgroundColor = colorDialog.currentColor
    }
}
