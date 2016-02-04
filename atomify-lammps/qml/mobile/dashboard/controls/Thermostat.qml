import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import Atomify 1.0

import "qrc:/mobile/style"
import "qrc:/mobile/dashboard"

DashboardControl {
    id: thermostatRoot

    // TODO add properties:
    // min and max temperature

    miniControl: Component {
        DashboardMiniControl {
            id: miniControl
            Column {
                anchors.centerIn: parent
                width: parent.width
                spacing: miniControl.height * 0.02
                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: "temperature\ntarget"
                    horizontalAlignment: Text.AlignHCenter
                    font.weight: Font.Light
                    font.pixelSize: miniControl.height * 0.12
                    color: "#cfcfcf"
                }
                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: nvt.targetTemperature.toFixed(2)
                    font.weight: Font.Light
                    font.pixelSize: miniControl.height * 0.3
                    color: "#ededed"
                }
                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: "thermostat"
                    font.weight: Font.Light
                    font.pixelSize: miniControl.height * 0.10
                    color: "#ababab"
                }
            }
        }
    }
    fullControl: Component {
        DashboardFullControl {
            Item {
                height: 1
                width: 1
                Layout.fillWidth: true
                Layout.fillHeight: true

                Column {
                    anchors.fill: parent
                    spacing: Style.spacing
                    Text {
                        font.pixelSize: Style.font.heading.size
                        color: Style.font.heading.color
                        text: "Thermostat"
                    }
                    Item {
                        anchors {
                            left: parent.left
                            right: parent.right
                        }
                        height: Style.touchableSize
                        Label {
                            anchors.left: parent.left
                            anchors.verticalCenter: parent.verticalCenter
                            text: "Enabled"
                            font.pixelSize: Style.font.size
                            color: Style.font.color
                        }
                        Switch {
                            id: nvtCheck
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.right: parent.right
                            checked: nvt.enabled
                            onCheckedChanged: {
                                nvt.enabled = checked
                            }
                        }
                    }
                    Text {
                        text: "Target:"
                        color: Style.font.color
                        font.pixelSize: Style.font.size
                    }
                    Item {
                        anchors {
                            left: parent.left
                            right: parent.right
                        }
                        height: Style.touchableSize
                        Slider {
                            id: nvtSlider
                            anchors {
                                left: parent.left
                                right: targetText.left
                                rightMargin: Style.spacing
                            }

                            minimumValue: 0.1
                            maximumValue: 6
                            value: nvt.targetTemperature
                            onValueChanged: {
                                nvt.targetTemperature = value
                            }
                        }
                        Text {
                            id: targetText
                            anchors {
                                right: parent.right
                            }
                            text: nvtSlider.value.toFixed(2)
                            color: Style.font.color
                            font.pixelSize: Style.font.size
                        }
                    }
                }
            }
            Item {
                height: 1
                width: 1
                Layout.fillWidth: true
                Layout.fillHeight: true

                Text {
                    id: previewText
                    text: "Preview"
                    color: Style.font.heading.color
                    font.pixelSize: Style.font.size
                }

                ShaderEffectSource{
                    property real aspectRatio: width / height
                    property real visualizerAspectRatio: visualizer ? visualizer.width / visualizer.height : 0

                    property real targetX: visualizer ? (visualizer.width - targetWidth) / 2 : 0
                    property real targetY: visualizer ? (visualizer.height - targetHeight) / 2 : 0

                    property real targetWidth: visualizer ? visualizer.width * ((aspectRatio > visualizerAspectRatio) ? 1.0 : (aspectRatio / visualizerAspectRatio)) : 0
                    property real targetHeight: visualizer ? visualizer.height * ((aspectRatio > visualizerAspectRatio) ? (visualizerAspectRatio / aspectRatio) : 1.0) : 0

                    anchors {
                        top: previewText.bottom
                        bottom: parent.bottom
                        left: parent.left
                        right: parent.right
                    }

                    sourceItem: visualizer
                    sourceRect: Qt.rect(targetX, targetY, targetWidth, targetHeight)
                }
            }

        }
    }
    simulatorControls: [
        NVT {
            id: nvt
            temperatureDampening: 0.01
            //            targetTemperature: nvtSlider.value
        }
    ]
}
