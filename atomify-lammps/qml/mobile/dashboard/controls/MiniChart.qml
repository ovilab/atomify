import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import Atomify 1.0
import QtCharts 2.0
import "qrc:/mobile/style"
import "qrc:/mobile/dashboard"

ChartView {
    id: miniChart

    property alias value: chartScroller.value
    property alias time: chartScroller.time
    property alias timeRange: chartScroller.timeRange
    property alias lowPassValue: chartScroller.lowPassValue
    
    antialiasing: true
    legend.visible: false
    backgroundColor: Style.chart.backgroundColor

    margins {
        top: 0
        bottom: 0
        left: 0
        right: 0
    }

    ChartScroller {
        id: chartScroller
        xAxis: xAxis
        yAxis: yAxis
        lineSeries: lineSeries
    }
    
    ValueAxis {
        id: xAxis
        tickCount: 0
        labelsVisible: false
        gridVisible: false
        visible: false
    }

    ValueAxis {
        id: yAxis
        tickCount: 0
        labelsVisible: false
        gridVisible: false
        visible: false
    }

    LineSeries {
        id: lineSeries
        width: 6.0
        color: "steelblue"
    }

    Component.onCompleted: {
        setAxisX(xAxis, lineSeries)
        setAxisY(yAxis, lineSeries)
    }

}
