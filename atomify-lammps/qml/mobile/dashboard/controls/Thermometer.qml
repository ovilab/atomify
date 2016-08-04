import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import Atomify 1.0
import QtCharts 2.0
import "qrc:/mobile/style"
import "qrc:/mobile/dashboard"
DashboardControl {
    id: root
    property real xRange: 0
    property string xLabel: "t [ps]"
    property string yLabel: "T [K]"
    property real xScale: 1.0
    property real yScale: 1.0
    property alias computeCommand: temperatureCompute.command
    name: "Temperature"
    fullControl: Column {
        ChartScrollerNew {
            id: miniChart
            title: "Temperature"
            xRange: root.xRange
            xScale: root.xScale
            yScale: root.yScale
            xLabel: root.xLabel
            yLabel: root.yLabel
            active: root.active

            anchors {
                left: parent.left
                right: parent.right
            }

            dataSources: [temperatureCompute.scalarValue]
            height: width * 2.5 / 4
        }
    }
    simulatorControls: [
        Compute {
            id: temperatureCompute
            scalarTitle: "Temperature"
            identifier: "temp"
            command: "all temp"
        }
    ]
}
