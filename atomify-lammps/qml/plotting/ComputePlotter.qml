import QtQuick 2.7
import QtQuick.Controls 2.0
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
        if(!compute) return
        updateSeries("line")
        for(var key in compute.data1D) {
            compute.data1D[key].updated.connect(updateGraphs(key))
            compute.data1D[key].xySeries = dataSeries[key]
        }
        title = "Compute '"+compute.identifier+"'"
        compute.willBeDestroyed.connect(function() {
            compute = null
            timer.stop()
            root.close()
        })
    }

    onVisibleChanged: {
        if(!visible) {
            if(!compute) return
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
            compute.data1D[key].updateXYSeries(dataSeries[key])
        }
    }

    function updateLimits() {
        if(!root.compute) return
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
        id: timer
        interval: 50
        repeat: true
        running: root.visible
        onTriggered: updateLimits()
    }

    function updateSeries(type) {
        chart.removeAllSeries();
        dataSeries = []

        if (type === "line") {
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

    Pane {
        id: rootPane
        anchors.fill: parent
        ColumnLayout {
            anchors.fill: parent
            spacing: 5
            ChartView {
                id: chart
                Layout.fillHeight: true
                Layout.fillWidth: true
                theme: ChartView.ChartThemeDark
                backgroundColor: Qt.rgba(0.3, 0.3, 0.3, 1.0)
                backgroundRoundness: 2.0
                antialiasing: true
                legend.visible: true
                titleColor: "black"
                title: compute ? compute.identifier : ""

                ValueAxis {
                    id: _axisX
                    tickCount: 3
                    min: 0
                    max: 100
                    titleText: compute ? compute.xLabel : ""
                    color: "white"
                    labelsColor: "gray"
                }

                ValueAxis {
                    id: _axisY
                    tickCount: 3
                    min: 0
                    max: 1000
                    titleText: compute ? compute.yLabel : ""
                    color: "white"
                    labelsColor: "gray"
                }
            }
            Row {
                Layout.fillWidth: true
                leftPadding: 12
                spacing: 5
                Button {
                    text: "Clear"
                    onClicked: root.compute.clear()
                }
                Button {
                    text: "Save data"
                    onClicked: console.log("Clear")
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
