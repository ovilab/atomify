import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import Atomify 1.0

import "qrc:/mobile/style"
import "qrc:/mobile/dashboard"

DashboardControl {
    id: thermostatRoot

    property real timeRange: 3

    // TODO add properties:
    // min and max temperature

    name: "Thermostat"
    miniControl: Component {
        DashboardMiniControl {
            id: miniControl
            ColumnLayout {
                id: textLayout
                anchors {
                    left: parent.left
                    top: parent.top
                    margins: Style.baseMargin * 0.5
                    right: parent.right
                    bottom: parent.bottom
                }
                spacing: Style.baseMargin * 0.2
                Text {
                    id: temperatureText
                    font.weight: Font.Light
                    font.pixelSize: Style.font.size
                    color: "#cfcfcf"
                    text: "thermostat"
                }
                Text {
                    id: temperatureValueText
                    font.pixelSize: Style.font.size * 3
                    color: "#cdcdcd"
                    text: miniChart.lowPassValue.toFixed(2) + " → " + nvt.targetTemperature.toFixed(2)
                }
                AutoZoomMiniChart {
                    id: miniChart

                    Layout.fillHeight: true
                    Layout.fillWidth: true

                    width: 1
                    height: 1

                    value: temperatureCompute.value
                    time: temperatureCompute.time
                    timeRange: thermostatRoot.timeRange
                }
            }
        }
    }
    fullControl: Item {
        Column {
            anchors {
                fill: parent
            }
            spacing: Style.spacing
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

    simulatorControls: [
        NVT {
            id: nvt
            temperatureDampening: 0.01
            enabled: false
        },
        Compute {
            id: temperatureCompute
            identifier: "thermostat_temp"
            command: "all temp"
        }
    ]
}
