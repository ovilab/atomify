import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.0
import QtQuick.Window 2.2
import Atomify 1.0
import QtCharts 2.1
import QtQuick.Controls.Styles 1.4

Window {
    id: root
    property var dataSeries: []
    property Compute compute
    width: 500
    height: 500
    onComputeChanged: {
        updateSeries("line")
        for(var key in compute.data1D) {
            compute.data1D[key].updated.connect(updateGraphs(key))
        }
        title = "Compute '"+compute.identifier+"'"
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
        _axisY.min = (yMin>0) ? 0.95*yMin : 1.05*yMin
        _axisY.max = (yMax<0) ? 0.95*yMax : 1.05*yMax

        _axisY.applyNiceNumbers()
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

    Column {
        spacing: 5
        ChartView {
            id: chart
            width: root.width
            height: root.height * 0.8
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

        Row {
            Button {
                text: "Max count: "
                tooltip: "Sets a limit on how many data points that is plotted. Useful for limiting the x-axis in timeseries for i.e. only showing the last 100 timesteps."
                style: LabelButton { }
            }

            TextField {
                id: maxCountText
                validator: IntValidator {bottom: 0; top: 100000;}
                text: compute ? compute.maxCount : 0
                style: TextFieldStyle {
                    textColor: "black"
                    background: Rectangle {
                        radius: 2
                        implicitWidth: 100
                        implicitHeight: 24
                        border.color: "#333"
                        border.width: 1
                    }
                }
            }
            Button {
                text: "Apply"
                onClicked: {
                    compute.maxCount = parseInt(maxCountText.text)
                }
            }
        }
    }

    Shortcut {
        sequence: StandardKey.Close
        onActivated: {
            root.close()
        }
    }
}
