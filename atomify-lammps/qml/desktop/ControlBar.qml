import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2
import Atomify 1.0
import QtQuick.Layouts 1.3
import Qt.labs.settings 1.0
import "../visualization"

Rectangle {
    id: root
    property AtomifySimulator simulator
    property AtomifyVisualizer visualizer
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
                tickmarksEnabled: true
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
