import MySimulator 1.0
import QtQuick 2.2
import QtQuick.Controls 1.1
import SimVis 1.0

Rectangle {
    id: simulatorControlRoot
    property MySimulator simulator
    width: 320
    height: sliders.height + okButton.height + 10
    color: "#ffffff"
    opacity: 0.9

    onVisibleChanged: {
        focus = visible
    }

    onSimulatorChanged: {
        massSlider.value = simulator.mass
        springConstantSlider.value = simulator.springConstant
        dtSlider.value = Math.log(simulator.dt)*Math.LOG10E // Set slider value to log of dt
        ballCount.text = simulator.ballCount
    }

    GroupBox {
        id: sliders
        width: parent.width
        height: 142
        title: qsTr("Simulation")

        Label {
            id: massLabel
            text: "Mass ("+simulator.mass.toFixed(1)+")"
            width: springConstantLabel.width
        }

        Slider {
            id: massSlider
            anchors.left: massLabel.right
            anchors.leftMargin: 5
            maximumValue: 5.0
            minimumValue: 0.1
            value: 1.0
            onValueChanged: {
                if(simulator != null) simulator.mass = value
            }
        }

        Label {
            id: dtLabel
            anchors.top: massLabel.bottom
            text: "dt ("+simulator.dt.toFixed(4)+")"
            width: springConstantLabel.width
        }

        Slider {
            id: dtSlider
            anchors.left: dtLabel.right
            anchors.top: dtLabel.top
            anchors.leftMargin: 5
            maximumValue: -1
            minimumValue: -4
            value: -3
            onValueChanged: {
                if(simulator != null) simulator.dt = Math.pow(10, value)
            }
        }

        Label {
            id: springConstantLabel
            anchors.top: dtLabel.bottom
            text: "Spring constant ("+simulator.springConstant.toFixed(1)+")"
        }

        Slider {
            id: springConstantSlider
            anchors.top: springConstantLabel.top
            anchors.left: springConstantLabel.right
            anchors.leftMargin: 5
            maximumValue: 5.0
            minimumValue: 0.0
            value: 1.0
            onValueChanged: {
                if(simulator != null) simulator.springConstant = value
            }
        }

        Label {
            id: ballCountLabel
            width: springConstantLabel.width
            anchors.top: springConstantLabel.bottom
            anchors.topMargin: 5
            text: "Ball count"
        }

        TextField {
            id: ballCount
            anchors.top: ballCountLabel.top
            anchors.left: ballCountLabel.right
            anchors.leftMargin: 5
            text: simulator.ballCount
            inputMethodHints: Qt.ImhFormattedNumbersOnly
            onTextChanged: {
                var value = Math.round(parseInt(text))
                simulator.ballCount = value
                simulator.reset()
            }
        }
    }

    Button {
        id: okButton
        x: parent.width*0.5 - width*0.5
        anchors.top: sliders.bottom
        anchors.topMargin: 5
        text: qsTr("OK")
        onClicked: simulatorControlRoot.visible = false
    }

    Keys.onPressed: {
        if(event.key === Qt.Key_Escape) {
            visible = false
        }
    }
}
