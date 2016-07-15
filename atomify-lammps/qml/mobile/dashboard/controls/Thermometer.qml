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

    name: "Temperature"
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
                AutoZoomMiniChart {
                    id: miniChart

                    Layout.fillHeight: true
                    Layout.fillWidth: true

                    width: 1
                    height: 1

                    value: temperatureCompute.value
                    time: temperatureCompute.time
                    timeRange: root.timeRange
                }
            }
        }
    }
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
