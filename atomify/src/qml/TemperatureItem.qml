import QtQuick 2.6
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.4
import QtCharts 2.1
import Atomify 1.0
import "mobile/dashboard/controls"

Rectangle {
    property alias compute: temperature
    property System system
    property var simulatorControls: [
        temperature
    ]

    width: parent.columns===1 ? parent.width : parent.width*0.5
    height: width

    ChartView {
        anchors.fill: parent
        antialiasing: true
        legend.visible: true
        title: "Temperature"
        titleColor: "black"

        LineSeries {
            id: lineSeries
            width: 1.0
            axisX: _axisX
            axisY: _axisY
        }

        ValueAxis {
            id: _axisX
            tickCount: 3
            min: 0
            max: 100
            titleText: "Time"
            color: "white"
            labelsColor: "black"
        }

        ValueAxis {
            id: _axisY
            tickCount: 3
            min: 0
            max: 1000
            titleText: "Temperature"
            color: "white"
            labelsColor: "black"
        }

        ChartScroller {
            id: chartScroller
            timeRange: 0.1

            axisX: _axisX
            axisY: _axisY
            lineSeries: lineSeries
        }
    }

    Compute {
        id: temperature
        identifier: "temperature"
        command: "temp"
        onValueChanged: {
            chartScroller.time = time
            chartScroller.value = value
        }
    }
}
