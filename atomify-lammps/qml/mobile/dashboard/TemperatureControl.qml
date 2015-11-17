import QtQuick 2.5
import QtQuick.Controls 1.4
import Atomify 1.0

DashboardControl {
    fullControl: Component {
        Column {
            Row {
                CheckBox {
                    id: nvtCheck
                    checked: nvt.enabled
                    onCheckedChanged: {
                        nvt.enabled = checked
                    }
                }
                Label {
                    text: "Enabled"
                }
            }
            Slider {
                id: nvtSlider
                minimumValue: 0.1
                maximumValue: 6
                value: nvt.targetTemperature
                onValueChanged: {
                    nvt.targetTemperature = value
                }
            }
        }
    }

    fixes: [
        NVT {
            id: nvt
            temperatureDampening: 0.01
            onEnabledChanged: {
                nvtCheck.checked = enabled
            }
        }
    ]

    Rectangle {
        anchors.fill: parent
        color: "purple"
    }
}
