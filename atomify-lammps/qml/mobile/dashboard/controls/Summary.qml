import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import Atomify 1.0
import QtCharts 2.0
import "qrc:/mobile/style"
import "qrc:/mobile/dashboard"
DashboardControl {
    id: root
    property System system
    name: "Summary"
    fullControl: Column {
        ChartView {
            id: chart
            antialiasing: true
            theme: ChartView.ChartThemeDark

            anchors {
                left: parent.left
                right: parent.right
            }

            height: width * 2.5 / 4
            Item {
                anchors.fill: parent
                anchors.margins: 20
                Column {
                    Text {
                        font.bold: true
                        color: "white"
                        text: "System size: ("+system.size.x.toFixed(1)+", "+system.size.y.toFixed(1)+", "+system.size.z.toFixed(1)+")"
                    }
                    Text {
                        font.bold: true
                        color: "white"
                        text: "System volume: "+system.volume.toFixed(0)
                    }
                    Text {
                        font.bold: true
                        color: "white"
                        text: "Number of atoms: "+root.system.numberOfAtoms
                    }
                    Text {
                        font.bold: true
                        color: "white"
                        text: "Number of atom types: "+system.numberOfAtomTypes
                    }
                    Text {
                        font.bold: true
                        color: "white"
                        text: "Timesteps: "+system.timesteps
                    }
                    Text {
                        font.bold: true
                        color: "white"
                        text: "Performance: "+(system.timesteps / (system.realTime / 1000)).toFixed(2)+" timesteps/sec"
                    }
                    Text {
                        font.bold: true
                        color: "white"
                        text: "Memory usage: "+ (system.memoryUsage / 1024 / 1024).toFixed(0) +" MB"
                    }
                }
            }
        }
    }
}
