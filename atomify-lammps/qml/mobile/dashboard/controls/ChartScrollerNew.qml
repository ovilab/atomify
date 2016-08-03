import QtQuick 2.5
import QtCharts 2.0
import Atomify 1.0
ChartView {
    id: root
    title: ""
    antialiasing: true
    legend.visible: false
    theme: ChartView.ChartThemeDark

    property bool autoScroll: true
    property alias xLabel: xAxis.titleText
    property alias yLabel: yAxis.titleText
    property alias xAxis: xAxis
    property alias yAxis: yAxis
    property string type: "line"
    property var dataSeries: [] // Will be updated automatically
    property list<Data1D> dataSources
    property real xRange: 0

    function updateData() {
        if(autoScroll) {
            updateLimits()
        }
        for(var i=0; i < dataSources.length; i++) {
            dataSources[i].updateData(dataSeries[i])
        }
    }

    function updateLimits() {
        if(dataSources.length === 0) return;
        dataSources[0].updateLimits()
        var xMin = dataSources[0].xMin
        var xMax = dataSources[0].xMax
        var yMin = dataSources[0].yMin
        var yMax = dataSources[0].yMax

        for(var i=0; i<dataSources.length; i++) {
            var data = dataSources[i]
            data.updateLimits()
            xMin = Math.min(xMin, data.xMin)
            xMax = Math.max(xMax, data.xMax)
            yMin = Math.min(yMin, data.yMin)
            yMax = Math.max(yMax, data.yMax)
        }

        xAxis.min = xMin
        xAxis.max = xMax
        yAxis.min = (yMin>0) ? 0.95*yMin : 1.05*yMin
        yAxis.max = (yMax<0) ? 0.95*yMax : 1.05*yMax
        // yAxis.applyNiceNumbers()
    }

    onDataSourcesChanged: {
        updateSeries()
        for(var i=0; i<dataSources.length; i++) {
            dataSources[i].xRange = root.xRange
            dataSources[i].updated.connect(updateData)
        }
    }

    Component.onDestruction: {
        for(var i in dataSources) {
            dataSources[i].updated.disconnect(updateData)
        }
    }

    ValueAxis {
        id: xAxis
        tickCount: 4
        color: "white"
        labelsColor: "white"
    }

    ValueAxis {
        id: yAxis
        tickCount: 4
        color: "white"
        labelsColor: "white"
    }

    function updateSeries() {
            root.removeAllSeries();
            dataSeries = []

            // Create two new series of the correct type. Axis x is the same for both of the series,
            // but the series have their own y-axes to make it possible to control the y-offset
            // of the "signal sources".
            if (type == "line") {
                for(var i=0; i<dataSources.length; i++) {
                    var series = root.createSeries(ChartView.SeriesTypeLine, i, xAxis, yAxis);
                    series.useOpenGL = true
                    dataSeries[i] = series
                }
            } else {
                for(i=0; i<dataSources.length; i++) {
                    series = root.createSeries(ChartView.SeriesTypeScatter, i, _axisX, _axisY);
                    series.useOpenGL = true
                    dataSeries[i] = series
                }
            }
        }
}
