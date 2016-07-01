import QtQuick 2.7
import QtQuick.Controls 1.4
import Atomify 1.0
import QtQuick.Layouts 1.3

Rectangle {
    id: root
    property AtomifySimulator simulator
    opacity: 0.9
    color: "#ccc"
    radius: 2

    RowLayout {
        width: parent.width-10
        height: parent.height-10
        x:5
        y:5
        spacing: 5
        Button {
            Layout.minimumHeight: 40
            Layout.minimumWidth: 40
            onClicked: {
                if(simulator != undefined) {
                    simulator.paused = !simulator.paused
                }
            }
            iconSource: simulator ? (simulator.paused ? "qrc:/images/play2.png" : "qrc:/images/pause2.png") : "qrc:/images/play2.png"
        }
        Column {
            Layout.fillHeight: true
            Layout.fillWidth: true
            Label {
                width: parent.width
                text: "Simulation speed ("+speedSlider.value+"x)"
            }

            Slider {
                id: speedSlider
                width: parent.width
                minimumValue: 1
                maximumValue: 100
                stepSize: 1
                value: simulator ? simulator.simulationSpeed : 1
                onValueChanged: {
                    if(simulator != undefined) {
                        simulator.simulationSpeed = value
                    }
                }
            }
        }

        Button {
            Layout.minimumHeight: 40
            Layout.minimumWidth: 40
            onClicked: {

            }
            iconSource: "qrc:/images/camera.png"
        }
    }
}
