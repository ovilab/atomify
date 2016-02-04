import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import Atomify 1.0
import QtCharts 2.0
import "qrc:/mobile/style"
import "qrc:/mobile/dashboard"
DashboardControl {
    id: dashboardControlRoot
    miniControl: Component {
        DashboardMiniControl {
            id: miniControl
            Layout.columnSpan: 2

            property real value: temperatureCompute.value
            property real lowPassValue: 0.0
            property real yMin: 0
            property real yMax: 0

            onValueChanged: {
                if(isNaN(value)) {
                    return;
                }
                var factor = 0.98
                lowPassValue = factor * lowPassValue + (1 - factor) * value
                lineSeries.append(temperatureCompute.time, lowPassValue)
                if(lineSeries.count > 1000) {
                    var firstPoint = lineSeries.at(0)
                    lineSeries.remove(firstPoint.x, firstPoint.y)
                }

                yMax = Math.max(yMax, lowPassValue)
                yMin = Math.min(yMin, lowPassValue)
                xAxis.max = temperatureCompute.time
                xAxis.min = lineSeries.at(0).x
                yAxis.min = yMin*0.9
                yAxis.max = yMax*1.1
            }
            ChartView {
                id: chart
                anchors.fill: parent
                antialiasing: true
                legend.visible: false
                ValueAxis {
                    id: xAxis
                    min: 0
                    max: 1
                    tickCount: 0
                    labelsVisible: false
                    gridVisible: false
                    visible: false
                }
                ValueAxis {
                    id: yAxis
                    min: 0
                    max: 0.2
                    tickCount: 0
                    labelsVisible: false
                    gridVisible: false
                    visible: false
                }
                LineSeries {
                    id: lineSeries
                }
                Component.onCompleted: {
                    chart.setAxisX(xAxis, lineSeries)
                    chart.setAxisY(yAxis, lineSeries)
                }
            }
            Column {
                anchors.centerIn: parent
                width: parent.width
                spacing: miniControl.height * 0.02
                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: "temperature"
                    horizontalAlignment: Text.AlignHCenter
                    font.weight: Font.Light
                    font.pixelSize: miniControl.height * 0.12
                    color: "#cfcfcf"
                }
                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: miniControl.lowPassValue.toFixed(2)
                    font.weight: Font.Light
                    font.pixelSize: miniControl.height * 0.3
                    color: "#ededed"
                }
                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: "thermometer"
                    font.weight: Font.Light
                    font.pixelSize: miniControl.height * 0.10
                    color: "#ababab"
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

            onValueChanged: {
                if(isNaN(value)) {
                    return;
                }

                var factor = 0.98
                lowPassValue = factor * lowPassValue + (1 - factor) * value
                lineSeries.append(temperatureCompute.time, lowPassValue)
                if(lineSeries.count > 1000) {
                    var firstPoint = lineSeries.at(0)
                    lineSeries.remove(firstPoint.x, firstPoint.y)
                }

                yMax = Math.max(yMax, lowPassValue)
                yMin = Math.min(yMin, lowPassValue)
                xAxis.max = temperatureCompute.time
                xAxis.min = lineSeries.at(0).x
                yAxis.min = yMin*0.9
                yAxis.max = yMax*1.1
            }

            ChartView {
                id: chart
                anchors.fill: parent
                antialiasing: true
                legend.visible: false
                ValueAxis {
                    id: xAxis
                    min: 0
                    max: 1
                    tickCount: 3
                }
                ValueAxis {
                    id: yAxis
                    min: 0
                    max: 0.2
                    tickCount: 3
                }
                LineSeries {
                    id: lineSeries
                }
                Component.onCompleted: {
                    chart.setAxisX(xAxis, lineSeries)
                    chart.setAxisY(yAxis, lineSeries)
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
