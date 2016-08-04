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
    property real maximumValue: 100.0
    property alias temperatureDampening: nvt.temperatureDampening
    property bool nvtEnabled: false
    property real targetTemperature: 100.0
    // TODO add properties:
    // min and max temperature

    name: "Thermostat"
    fullControl: Item {
        height: 500

        Column {
            id: column
            anchors.fill: parent
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
    }

    simulatorControls: [
        NVT {
            id: nvt
            temperatureDampening: 1.0
            enabled: root.nvtEnabled
            targetTemperature: root.targetTemperature
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
