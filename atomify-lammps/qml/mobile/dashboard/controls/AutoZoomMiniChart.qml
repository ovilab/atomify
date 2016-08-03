import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import Atomify 1.0
import QtCharts 2.0
import "qrc:/mobile/style"
import "qrc:/mobile/dashboard"

MiniChart {
    id: miniChart

    property alias value: chartScroller.value
    property alias time: chartScroller.time
    property alias timeRange: chartScroller.timeRange
    property alias lowPassValue: chartScroller.lowPassValue

    property alias yMin: chartScroller.yMin
    property alias yMax: chartScroller.yMax
    
    LineSeries {
        id: lineSeries
        width: 6.0
        color: "steelblue"
        axisX: miniChart.axisX
        axisY: miniChart.axisY
        useOpenGL: true
    }
    
    ChartScroller {
        id: chartScroller
        axisX: miniChart.axisX
        axisY: miniChart.axisY
        lineSeries: lineSeries
    }
}
