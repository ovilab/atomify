import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Window 2.0
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0
import MySimulator 1.0
import SimVis 1.0
import Compute 1.0
import LammpsOutput 1.0

ApplicationWindow {
    id: applicationRoot
    title: qsTr("LAMMPS live visualization")
    width: 1650
    height: 900
    visible: true

//    Component.onCompleted: {
//        visualizer.simulator.lammpsOutput = output
//    }

    SplitView {
        anchors.fill: parent
        Layout.alignment: Qt.AlignTop
        Layout.fillHeight: true

        TabView {
            width: applicationRoot.width*0.4
            height: parent.height

            Tab {
                anchors.fill: parent
                title: "Script editor"

                LammpsEditor {
                    anchors.fill: parent
                    id: lammpsEditor
                    simulator: visualizer.simulator
                }
            }

            Tab {
                anchors.fill: parent
                title: "Demo simulations"

                DemoSimulations {
                    anchors.fill: parent
                    id: demoSimulations
                    simulator: visualizer.simulator
                }
            }
        }
        AtomifyVisualizer {
            id: visualizer
            Layout.alignment: Qt.AlignLeft
            Layout.fillHeight: true
            Layout.fillWidth: true
        }
    }

    Compute {
        id: temperature
        identifier: "temperature"
        command: "compute temperature all temp"
        simulator: visualizer.simulator
    }

    Compute {
        id: pressure
        identifier: "pressure"
        command: "compute pressure all pressure temperature"
        simulator: visualizer.simulator
        dependencies: ["temperature"]
    }
}
