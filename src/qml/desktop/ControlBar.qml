import QtQuick 2.7
import QtQuick.Controls 2.1
import QtQuick.Controls 1.4 as QQC1
import QtQuick.Dialogs 1.2
import Atomify 1.0
import QtQuick.Layouts 1.3
import Qt.labs.settings 1.0
import "../visualization"

Rectangle {
    id: root
    property AtomifySimulator simulator
    property AtomifyVisualizer visualizer
    property bool hidden: true
    opacity: hidden ? 0.0 : 1.0
    Behavior on opacity {
        NumberAnimation {
            duration: 200
            easing.type: Easing.OutBounce
        }
    }

    color: "#444"
    radius: 2

    QQC1.Button {
        id: pauseButton
        anchors {
            left: parent.left
            top: parent.top
            bottom: parent.bottom
        }
        enabled: simulator.states.parsing.active || simulator.states.continued.active || simulator.states.paused.active || simulator.states.finished.active
        width: height
        onClicked: {
            simulator.togglePause()
        }
        iconSource: {
            if(!simulator) {
                return "qrc:/images/play2.png"
            }
            if(simulator.states.paused.active || simulator.states.finished.active || simulator.states.idle.active) {
                return "qrc:/images/play2.png"
            } else {
                return "qrc:/images/pause2.png"
            }
        }
    }
    Label {
        id: label
        anchors {
            left: pauseButton.right
            top: parent.top
            right: screenshotButton.left
            leftMargin: 12
            rightMargin: 12
            topMargin: 8
        }
        horizontalAlignment: Label.AlignHCenter
        text: "Simulation speed ("+speedSlider.value+"x)"
    }
    Slider {
        id: speedSlider

        anchors {
            left: pauseButton.right
            bottom: parent.bottom
            right: screenshotButton.left
            leftMargin: 12
            rightMargin: 12
        }
        from: 1
        to: 100
        stepSize: 1
        value: simulator ? simulator.simulationSpeed : 1
        onValueChanged: {
            if(simulator != undefined) {
                simulator.simulationSpeed = value
            }
        }
    }

    QQC1.Button {
        id: screenshotButton
        anchors {
            right: parent.right
            top: parent.top
            bottom: parent.bottom
        }
        width: height
        onClicked: {
            visualizer.grabToImage(function(result) {
                fileDialogSave.cb = function() {
                    // TODO: figure out why file:// doesn't work for saveToFile function
                    result.saveToFile(fileDialogSave.fileUrl.toString().replace("file://",""));
                }
                fileDialogSave.visible = true
            })
        }
        iconSource: "qrc:/images/camera.png"
    }

    Settings {
        property alias lastOpenedFolder: fileDialogSave.folder
    }

    FileDialog {
        id: fileDialogSave
        selectExisting : false
        property var cb
        title: "Please choose a location to save"

        nameFilters: [ "Image files (*.jpg *.png)" ]


        onAccepted: {
            if(cb != undefined) {
                cb()
                cb = null
            }
        }
    }
}
