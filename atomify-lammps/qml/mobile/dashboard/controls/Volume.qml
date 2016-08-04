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
    property string yLabel: "Volume [Å<sup>3</sup>]"
    property real xScale: 1.0
    property real yScale: 1.0
    property string type: "line"
    name: "Volume"
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
            type: root.type

            anchors {
                left: parent.left
                right: parent.right
            }

            dataSources: [variable.data]
            height: width * 2.5 / 4
        }
    }
    simulatorControls: [
        Variable {
            id: variable
            title: "Volume"
            identifier: "volume"
            command: "equal vol"
        }
    ]
}
