import QtQuick 2.5
import QtQuick.Layouts 1.2
import Atomify 1.0

Rectangle {
    property System system
//    property Compute pressure
//    property Compute temperature
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
            text: "System size: ("+system.size.x.toFixed(1)+", "+system.size.y.toFixed(1)+", "+system.size.z.toFixed(1)+")"
        }
        Text {
            font.bold: true
            text: "Number of atoms: "+system.numberOfAtoms
        }
        Text {
            font.bold: true
            text: "Number of atom types: "+system.numberOfAtomTypes
        }

        Text {
            font.bold: true
            text: "Timesteps: "+system.timesteps
        }

        Text {
            font.bold: true
            text: "Time: "+system.simulationTime.toFixed(2)
        }
    }

//    MouseArea {
//        anchors.fill: parent
//        drag.target: parent
//        drag.axis: Drag.XAndYAxis
//    }
}
