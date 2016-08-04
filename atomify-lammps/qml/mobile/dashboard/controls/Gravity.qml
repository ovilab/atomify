import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import Atomify 1.0

import "qrc:/mobile/controls"
import "qrc:/mobile/dashboard"
import "qrc:/mobile/style"

DashboardControl {
    id: root
    property int precision: 1
    property real minimumValue: -1.0
    property real maximumValue: 1.0
    property real initialValue: 1.0
    property real unitScale: 1.0
    name: "Force"
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
                    id: enabledCheck
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.right: parent.right
                    checked: fix.enabled
                    onCheckedChanged: {
                        fix.enabled = checked
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
                target: fix
                property: "magnitude"
                text: "Magnitude"
            }
        }
    }

    simulatorControls: [
        Fix {
            id: fix
            property real magnitude: root.initialValue
            onMagnitudeChanged: {
                console.log("New magnitude: ", magnitude)
            }

            identifier: "gravity"
            enabled: root.enabled
            command: "mobile gravity "+magnitude+" vector 0 0 1"
            onCommandChanged: {
                forceReset = true
            }
        }
    ]
}
