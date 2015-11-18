import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import Atomify 1.0

import "qrc:/mobile/style"

DashboardControl {
    miniControl: DashboardMiniControl {
        Rectangle {
            anchors.fill: parent
            color: "purple"
        }
    }
    fullControl: DashboardFullControl {
        Item {
            Layout.fillHeight: true
            Layout.fillWidth: true
            Column {
                anchors.fill: parent
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
                        font.pixelSize: Style.font.size
                        color: Style.font.color
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
        ShaderEffectSource{
            property real aspectRatio: width / height
            property real visualizerAspectRatio: visualizer ? visualizer.width / visualizer.height : 0

            property real targetX: visualizer ? (visualizer.width - targetWidth) / 2 : 0
            property real targetY: visualizer ? (visualizer.height - targetHeight) / 2 : 0

            property real targetWidth: visualizer ? visualizer.width * ((aspectRatio > visualizerAspectRatio) ? 1.0 : (aspectRatio / visualizerAspectRatio)) : 0
            property real targetHeight: visualizer ? visualizer.height * ((aspectRatio > visualizerAspectRatio) ? (visualizerAspectRatio / aspectRatio) : 1.0) : 0

            Layout.fillHeight: true
            Layout.fillWidth: true

            sourceItem: visualizer
            sourceRect: Qt.rect(targetX, targetY, targetWidth, targetHeight)
        }
    }
    fixes: [
        NVT {
            id: nvt
            temperatureDampening: 0.01
            //            onEnabledChanged: {
            //                nvtCheck.checked = enabled
            //            }
        }
    ]
}
