import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import QtQuick.Window 2.2
import Atomify 1.0
import QtCharts 2.1

Window {
    id: root
    property var dataSeries: []
    property Compute compute
    width: 500
    height: 500
    // flags: Qt.Popup
    onComputeChanged: {
        updateSeries("line")
        for(var key in compute.data1D) {
            compute.data1D[key].updated.connect(updateGraphs(key))
        }
        title = "Timeseries of compute "+compute.identifier
    }

    onVisibleChanged: {
        if(!visible) {
            for(var key in compute.data1D) {
                compute.data1D[key].updated.disconnect(updateGraphs(key))
            }
        } else {
            updateLimits()
        }
    }

    function updateGraphs(key) {
        return function() {
            if(!root.visible) return;
            compute.data1D[key].updateData(dataSeries[key])
        }
    }

    function updateLimits() {
        var xMin = 1e9
        var xMax = -1e9
        var yMin = 1e9
        var yMax = -1e9
        for(var key in compute.data1D) {
            var data = compute.data1D[key]
            data.updateLimits()
            xMin = Math.min(xMin, data.xMin)
            xMax = Math.max(xMax, data.xMax)
            yMin = Math.min(yMin, data.yMin)
            yMax = Math.max(yMax, data.yMax)
        }
        _axisX.min = xMin
        _axisX.max = xMax
        _axisY.min = yMin
        _axisY.max = yMax
    }

    Timer {
        interval: 50
        repeat: true
        running: root.visible
        onTriggered: updateLimits()
    }

    function updateSeries(type) {
        chart.removeAllSeries();
        dataSeries = []

        // Create two new series of the correct type. Axis x is the same for both of the series,
        // but the series have their own y-axes to make it possible to control the y-offset
        // of the "signal sources".
        if (type == "line") {
            for(var key in compute.data1D) {
                var series = chart.createSeries(ChartView.SeriesTypeLine, key, _axisX, _axisY);
                dataSeries[key] = series
            }
        } else {
            for(key in compute.data1D) {
                series = chart.createSeries(ChartView.SeriesTypeScatter, key, _axisX, _axisY);
                dataSeries[key] = series
            }
        }
    }

    Rectangle {
        anchors.fill: parent
        border.color: "black"
        border.width: 1

        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            onHoveredChanged: {
                if(!containsMouse) {
                    root.hide()
                }
            }
        }

        ChartView {
            id: chart
            anchors.fill: parent
            antialiasing: true
            legend.visible: true
            title: compute ? compute.identifier : ""
            titleColor: "black"

            ValueAxis {
                id: _axisX
                tickCount: 3
                min: 0
                max: 100
                titleText: compute ? compute.xLabel : ""
                color: "white"
                labelsColor: "black"
            }

            ValueAxis {
                id: _axisY
                tickCount: 3
                min: 0
                max: 1000
                titleText: compute ? compute.yLabel : ""
                color: "white"
                labelsColor: "black"
            }
        }
    }
}
