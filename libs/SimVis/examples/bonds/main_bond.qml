import QtQuick 2.6
import QtQuick.Controls 1.4

Item {
    id: applicationRoot
    width: 1650
    height: 900
    visible: true

    BondScene {
        id: scene
        anchors.fill: parent
        focus: true
    }

    Shortcut {
        sequence: StandardKey.Copy
        onActivated: {
            scene.doSomething()
        }
    }

    Column {
        Row {
            Label {
                text: "Radius 1: "
            }

            Slider {
                minimumValue: 0.2
                maximumValue: 2.0
                stepSize: 0.1
                value: 1.0
                onValueChanged: {
                    scene.simulator.radius1 = value
                }
            }
        }

        Row {
            Label {
                text: "Radius 2: "
            }

            Slider {
                minimumValue: 0.2
                maximumValue: 2.0
                stepSize: 0.1
                value: 1.0
                onValueChanged: {
                    scene.simulator.radius2 = value
                }
            }
        }
    }
}

