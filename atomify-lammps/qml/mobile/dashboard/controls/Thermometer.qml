import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import Atomify 1.0

import "qrc:/mobile/style"
import "qrc:/mobile/dashboard"

DashboardControl {
    miniControl: Component {
        DashboardMiniControl {
            id: miniControl
            Column {
                anchors.centerIn: parent
                width: parent.width
                spacing: miniControl.height * 0.02
                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: "temperature"
                    horizontalAlignment: Text.AlignHCenter
                    font.weight: Font.Light
                    font.pixelSize: miniControl.height * 0.12
                    color: "#cfcfcf"
                }
                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: temperatureCompute.lowPassValue.toFixed(2)
                    font.weight: Font.Light
                    font.pixelSize: miniControl.height * 0.3
                    color: "#ededed"
                }
                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: "thermometer"
                    font.weight: Font.Light
                    font.pixelSize: miniControl.height * 0.10
                    color: "#ababab"
                }
            }
        }
    }
    simulatorControls: [
        Compute {
            id: temperatureCompute
            property real lowPassValue: 0.0
            identifier: "temp"
            command: "all temp"
            onValueChanged: {
                var factor = 0.98
                lowPassValue = factor * lowPassValue + (1 - factor) * value
            }
        }
    ]
}
