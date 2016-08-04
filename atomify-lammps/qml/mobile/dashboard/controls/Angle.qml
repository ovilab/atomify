import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import Atomify 1.0
import QtCharts 2.0
import "qrc:/mobile/style"
import "qrc:/mobile/dashboard"
DashboardControl {
    id: root
    property string title: "Angle"
    property string xLabel: "t"
    property string yLabel: "Angle &thetasym;"
    property real xRange: 0
    property real xScale: 1.0
    property real yScale: 1.0
    property int atom1
    property int atom2
    property int atom3
    property alias compute: compute
    property alias computeCommand: compute.command

    name: "Angle"
    fullControl: Column {
        ChartScrollerNew {
            id: miniChart
            title: root.title
            xLabel: root.xLabel
            yLabel: root.yLabel
            xRange: root.xRange
            xScale: root.xScale
            yScale: root.yScale
            showLegend: true
            active: root.active
            autoScroll: true

            anchors {
                left: parent.left
                right: parent.right
            }
            dataSources: [ compute.scalarValue, compute2.scalarValue ]
            height: width * 2.5 / 4
        }
    }
    simulatorControls: [
        Compute {
            id: compute
            scalarTitle: "Current"
            identifier: "angle"
            command: "all angle/atoms "+root.atom1+" "+root.atom2+" "+root.atom3
        },
        Compute {
            id: compute2
            scalarTitle: "Average"
            identifier: "angle2"
            command: "all angle/atoms "+root.atom1+" "+root.atom2+" "+root.atom3+" average"
        }
    ]
}
