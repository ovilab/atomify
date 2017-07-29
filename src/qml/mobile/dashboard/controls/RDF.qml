import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import Atomify 1.0
import QtCharts 2.0
import "qrc:/mobile/style"
import "qrc:/mobile/dashboard"
DashboardControl {
//    id: root
//    property string xLabel: "t [ps]"
//    property string yLabel: "&lt;r<sup>2</sup>(t)&gt; [Å<sup>2</sup>]"
//    property real xRange: 0
//    property real xScale: 1.0
//    property real yScale: 1.0
//    property alias compute: compute
//    property alias computeCommand: compute.command

//    name: "MSD"
//    fullControl: Column {
//        ChartScrollerNew {
//            id: miniChart
//            title: "Temperature"
//            xRange: root.xRange
//            xScale: root.xScale
//            yScale: root.yScale
//            xLabel: root.xLabel
//            yLabel: root.yLabel
//            active: root.active
//            autoScroll: true

//            anchors {
//                left: parent.left
//                right: parent.right
//            }
//            dataSources: [ compute.vectorValues["r2"] ]
//            height: width * 2.5 / 4
//        }
//    }
//    simulatorControls: [
//        Compute {
//            id: compute
//            vectorTitles: ["x", "y", "z", "r2"]
//            scalarTitle: "MSD"
//            identifier: "msd"
//            command: "all msd"
//        }
//    ]
}
