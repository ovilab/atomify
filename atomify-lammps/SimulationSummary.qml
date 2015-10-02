import QtQuick 2.5
import QtQuick.Layouts 1.2
import MySimulator 1.0
import Compute 1.0

Rectangle {
    property MySimulator simulator
    property Compute pressure
    property Compute temperature
    x: 20
    y: 20
    width: statusColumn.width+20
    height: statusColumn.height+20
    radius: 4
    color: Qt.rgba(1.0, 1.0, 1.0, 0.75)
    ColumnLayout {
        y: 10
        x: 10
        id: statusColumn
        Text {
            font.bold: true
            text: "Number of atoms: "+simulator.numberOfAtoms
        }
        Text {
            font.bold: true
            text: "Number of atom types: "+simulator.numberOfAtomTypes
        }
        Text {
            font.bold: true
            text: "Time per timestep [ms]: "+simulator.timePerTimestep.toFixed(2)
        }

        Text {
            font.bold: true
            text: "Temperature: "+temperature.value
        }

        Text {
            font.bold: true
            text: "Pressure: "+pressure.value
        }
    }

    MouseArea {
        anchors.fill: parent
        drag.target: parent
        drag.axis: Drag.XAndYAxis
    }
}
