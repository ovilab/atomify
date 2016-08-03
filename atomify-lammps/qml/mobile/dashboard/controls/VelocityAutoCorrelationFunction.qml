import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import Atomify 1.0
import QtCharts 2.0
import "qrc:/mobile/style"
import "qrc:/mobile/dashboard"
DashboardControl {
    id: root
    property real timeRange: 4
    property string xLabel: "t [ps]"
    property string yLabel: "&lt;v(t)v(0)&gt;"
    property real xScale: 1.0
    property real yScale: 1.0
    property alias computeCommand: compute.command
    property alias compute: compute

    name: "VACF"
    fullControl: Column {
        ChartView {
            id: miniChart

            property real value: compute.value
            property real lowPassValue: 0.0
            property real yMin: 0
            property real yMax: 0
            title: "Velocity autocorrelation function"

            anchors {
                left: parent.left
                right: parent.right
            }
            height: width * 2.5 / 4

            antialiasing: true
            legend.visible: false
            theme: ChartView.ChartThemeDark

            ChartScroller {
                id: chartScroller
                axisX: xAxis
                axisY: yAxis
                lineSeries: lineSeries
                value: compute.value * yScale
                time: compute.time * xScale
                timeRange: root.timeRange * xScale
            }

            ValueAxis {
                id: xAxis
                tickCount: 4
                titleText: root.xLabel
                color: "white"
                labelsColor: "white"
            }
            ValueAxis {
                id: yAxis
                tickCount: 4
                titleText: root.yLabel
                color: "white"
                labelsColor: "white"
            }
            LineSeries {
                id: lineSeries
            }
            Component.onCompleted: {
                miniChart.setAxisX(xAxis, lineSeries)
                miniChart.setAxisY(yAxis, lineSeries)
            }
        }

//        Text {
//            text: "Hallo hva tror du."
//        }
    }
    simulatorControls: [
        Compute {
            id: compute
            identifier: "vacf_compute"
            command: "all vacf"
            isVector: true
        }
    ]
}
