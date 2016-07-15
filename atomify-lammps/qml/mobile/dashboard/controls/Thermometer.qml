import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import Atomify 1.0
import QtCharts 2.0
import "qrc:/mobile/style"
import "qrc:/mobile/dashboard"
DashboardControl {
    id: root
    property real timeRange: 1
    property string timeLabel: "t [ps]"
    property string temperatureLabel: "T [K]"
    property real timeScale: 1.0
    property real temperatureScale: 1.0
    property alias computeCommand: temperatureCompute.command

    name: "Temperature"
    fullControl: Column {
        ChartView {
            id: miniChart

            property real value: temperatureCompute.value
            property real lowPassValue: 0.0
            property real yMin: 0
            property real yMax: 0

            anchors {
                left: parent.left
                right: parent.right
            }
            height: width * 3 / 4

            antialiasing: true
            legend.visible: false
            theme: ChartView.ChartThemeDark

            ChartScroller {
                id: chartScroller
                axisX: xAxis
                axisY: yAxis
                lineSeries: lineSeries
                value: temperatureCompute.value * temperatureScale
                time: temperatureCompute.time * timeScale
                timeRange: root.timeRange * timeScale
            }

            ValueAxis {
                id: xAxis
                tickCount: 4
                titleText: root.timeLabel
                color: "white"
                labelsColor: "white"
            }
            ValueAxis {
                id: yAxis
                tickCount: 4
                titleText: root.temperatureLabel
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
    }
    simulatorControls: [
        Compute {
            id: temperatureCompute
            identifier: "temp"
            command: "all temp"
        }
    ]
}
