import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import Atomify 1.0
import QtCharts 2.0
import "qrc:/mobile/style"
import "qrc:/mobile/dashboard"

DashboardControl {
    miniControl: DashboardMiniControl {
        id: miniControl

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
                color: "#000"
            }
            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text: temperatureCompute.lowPassValue.toFixed(2)
                font.weight: Font.Light
                font.pixelSize: miniControl.height * 0.3
                color: "#000"
            }
            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "thermometer"
                font.weight: Font.Light
                font.pixelSize: miniControl.height * 0.10
                color: "#000"
            }
        }
    }

    fullControl: DashboardFullControl {
        Item {
            Layout.fillHeight: true
            Layout.fillWidth: true
            ChartView {
                id: chartFull
                anchors.fill: parent
                antialiasing: true
                legend.visible: false

                ValueAxis {
                    id: xAxisFull
                    min: 0
                    max: 1
                    tickCount: 5
                }

                ValueAxis {
                    id: yAxisFull
                    min: 0
                    max: 0.2
                    tickCount: 5
                }

                LineSeries {
                    id: lineSeriesFull
                }

                Component.onCompleted: {
                    chartFull.setAxisX(xAxisFull, lineSeriesFull)
                    chartFull.setAxisY(yAxisFull, lineSeriesFull)
                }
            }
        }
    }

    simulatorControls: [
        Compute {
            id: temperatureCompute
            property real lowPassValue: 0.0
            property real yMin: 1e9
            property real yMax: -1e9

            identifier: "temp"
            command: "all temp"
            onValueChanged: {
                var factor = 0.98
                lowPassValue = factor * lowPassValue + (1 - factor) * value

                lineSeries.append(time, lowPassValue)
                lineSeriesFull.append(time, lowPassValue)
                if(lineSeries.count > 1000) {
                    var firstPoint = lineSeries.at(0)
                    lineSeries.remove(firstPoint.x, firstPoint.y)
                    lineSeriesFull.remove(firstPoint.x, firstPoint.y)
                }

                yMax = Math.max(yMax, lowPassValue)
                yMin = Math.min(yMin, lowPassValue)
                xAxis.max = time
                xAxis.min = lineSeries.at(0).x
                xAxisFull.max = time
                xAxisFull.min = lineSeries.at(0).x
                yAxis.min = yMin*0.9
                yAxis.max = yMax*1.1
                yAxisFull.min = yMin*0.9
                yAxisFull.max = yMax*1.1
            }
        }
    ]
}
