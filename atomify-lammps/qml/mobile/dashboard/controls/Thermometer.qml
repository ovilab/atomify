import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import Atomify 1.0
import QtCharts 2.0
import "qrc:/mobile/style"
import "qrc:/mobile/dashboard"
DashboardControl {
    id: dashboardControlRoot
    property real timeRange: 3
    miniControl: Component {
        DashboardMiniControl {
            id: miniControl

            ColumnLayout {
                id: textLayout
                anchors {
                    left: parent.left
                    top: parent.top
                    margins: Style.baseMargin * 0.5
                    right: parent.right
                    bottom: parent.bottom
                }
                spacing: Style.baseMargin * 0.2
                Text {
                    id: temperatureText
                    font.weight: Font.Light
                    font.pixelSize: Style.font.size
                    color: "#cfcfcf"
                    text: "temperature"
                }
                Text {
                    id: temperatureValueText
                    font.pixelSize: Style.font.size * 3
                    color: "#cdcdcd"
                    text: miniChart.lowPassValue.toFixed(2)
                }
                MiniChart {
                    id: miniChart

                    Layout.fillHeight: true
                    Layout.fillWidth: true

                    width: 1
                    height: 1

                    value: temperatureCompute.value
                    time: temperatureCompute.time
                    timeRange: dashboardControlRoot.timeRange
                }
            }
        }
    }
    fullControl: Component {
        DashboardFullControl {
            property real value: temperatureCompute.value
            property real lowPassValue: 0.0
            property real yMin: 0
            property real yMax: 0
            Layout.fillHeight: true
            Layout.fillWidth: true

            ChartView {
                id: miniChart
                anchors.fill: parent
                antialiasing: true
                legend.visible: false
                title: "Temperature"
                backgroundColor: "transparent"
                titleColor: "white"

                ChartScroller {
                    id: chartScroller
                    xAxis: xAxis
                    yAxis: yAxis
                    lineSeries: lineSeries

                    value: temperatureCompute.value
                    time: temperatureCompute.time
                    timeRange: dashboardControlRoot.timeRange
                }

                ValueAxis {
                    id: xAxis
                    tickCount: 3
                    titleText: "t"
                    color: "white"
                    labelsColor: "white"
                }
                ValueAxis {
                    id: yAxis
                    tickCount: 3
                    titleText: "T [K]"
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
    }
    simulatorControls: [
        Compute {
            id: temperatureCompute
            identifier: "temp"
            command: "all temp"
        }
    ]
}
