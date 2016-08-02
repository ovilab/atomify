import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import Atomify 1.0

import "qrc:/mobile/controls"
import "qrc:/mobile/dashboard"
import "qrc:/mobile/style"

DashboardControl {
    id: root

    property real timeRange: 3
    property string unit: "K"
    property real unitScale: 1.0
    property int precision: 1
    property real minimumValue: 1.0
    property real maximumValue: 10.0
    property alias temperatureDampening: nvt.temperatureDampening

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
                    timeRange: root.timeRange
                }
            }
        }
    }
    fullControl: Column {
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
        BoundSlider {
            id: nvtSlider
            minimumValue: root.minimumValue / root.unitScale
            maximumValue: root.maximumValue / root.unitScale
            unitScale: 1.0 / root.unitScale
            precision: root.precision
            unit: root.unit

            target: nvt
            property: "targetTemperature"
            text: "Target temperature"
        }
    }

    simulatorControls: [
        NVT {
            id: nvt
            temperatureDampening: 1.0
            enabled: false
            onCommandChanged: {
                console.log("Command: ", command)
            }
        },
        Compute {
            id: temperatureCompute
            identifier: "thermostat_temp"
            command: "all temp"
        }
    ]
}
