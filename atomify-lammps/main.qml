import QtQuick 2.4
import QtQuick.Controls 1.4
import QtQuick.Window 2.0
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0
import MySimulator 1.0
import SimVis 1.0
import Compute 1.0
import LammpsOutput 1.0
import AtomStyle 1.0

ApplicationWindow {
    id: applicationRoot
    title: qsTr("Atomify LAMMPS - live visualization")
    width: 1650
    height: 900
    visible: true

    SplitView {
        anchors.fill: parent
        Layout.alignment: Qt.AlignTop
        Layout.fillHeight: true

        TabView {
            width: applicationRoot.width*0.4
            height: parent.height

            Tab {
                id: editorTab
                anchors.fill: parent
                title: "Script editor"

                LammpsEditor {
                    anchors.fill: parent
                    id: lammpsEditor
                    simulator: mySimulator
                }
            }

            Tab {
                id: renderingTab
                anchors.fill: parent
                title: "Rendering"
                Rendering {
                    anchors.fill: parent
                    simulator: mySimulator
                    light: myVisualizer.light
                }
            }
        }

        AtomifyVisualizer {
            id: myVisualizer
            simulator: mySimulator
            Layout.alignment: Qt.AlignLeft
            Layout.fillHeight: true
            Layout.fillWidth: true
        }
    }

    MySimulator {
        id: mySimulator
        simulationSpeed: 1
        atomStyle: AtomStyle {
            id: myAtomStyle
        }
    }

//    Compute {
//        id: temperature
//        simulator: mySimulator
//        identifier: "temperature"
//        command: "compute temperature all temp"
//        onFirstValueChanged: {
//            // editorTab.title = "t="+time.toFixed(2)+" T="+firstValue.toFixed(4)
//        }
//    }

//    Compute {
//        id: msd
//        simulator: mySimulator
//        identifier: "msd"
//        command: "compute msd all msd"
//        isVector: true
//        onValuesChanged: {
//            // console.log("Values: "+values)
//            editorTab.title = firstValue+" "+secondValue+" "+thirdValue+" "+fourthValue
//        }
//    }

//    Compute {
//        id: pressure
//        simulator: mySimulator
//        identifier: "pressure"
//        command: "compute pressure all pressure temperature"
//        dependencies: [temperature.identifier]
//    }
}
