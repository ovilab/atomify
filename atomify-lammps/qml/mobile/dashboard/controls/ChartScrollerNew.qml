import QtQuick 2.5
import QtCharts 2.0
import Atomify 1.0
ChartView {
    id: root
    title: ""
    antialiasing: true
    legend.visible: showLegend
    theme: ChartView.ChartThemeDark
    property bool showLegend: false
    property bool autoScrollX: true
    property bool autoScrollY: true
    property bool active: true
    property alias xLabel: xAxis.titleText
    property alias yLabel: yAxis.titleText
    property alias xMin: xAxis.min
    property alias xMax: xAxis.max
    property alias yMin: yAxis.min
    property alias yMax: yAxis.max
    property real xRange: 0
    property real xScale: 1.0
    property real yScale: 1.0
    property var colors: []
    property string type: "line"
    property var dataSeries: [] // Will be updated automatically
    property list<Data1D> dataSources

    onDataSourcesChanged: {
        updateSeries()
        for(var i=0; i<dataSources.length; i++) {
            dataSources[i].xScale = root.xScale
            dataSources[i].yScale = root.yScale
            dataSources[i].xRange = root.xRange
            dataSources[i].updated.connect(updateData)
            dataSeries[i].name = dataSources[i].title
        }
    }
    
    onXScaleChanged: {
        for(var i=0; i<dataSources.length; i++) {
            dataSources[i].xScale = root.xScale
        }
    }

    onYScaleChanged: {
        for(var i=0; i<dataSources.length; i++) {
            dataSources[i].yScale = root.yScale
        }
    }

    onXRangeChanged: {
        for(var i=0; i<dataSources.length; i++) {
            dataSources[i].xRange = root.xRange
        }
    }

    function updateData() {
        if(!root.active) {
            return
        }

        if(autoScrollX || autoScrollY) {
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

        if(autoScrollX) {
            xAxis.min = xMin
            xAxis.max = xMax
        }
        if(autoScrollY) {
            yAxis.min = (yMin>0) ? 0.95*yMin : 1.05*yMin
            yAxis.max = (yMax<0) ? 0.95*yMax : 1.05*yMax
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
                    // series.useOpenGL = true
                    dataSeries[i] = series
                    if(root.colors.length > i) {
                        series.color = root.colors[i]
                    }
                }
            } else {
                for(i=0; i<dataSources.length; i++) {
                    series = root.createSeries(ChartView.SeriesTypeScatter, i, _axisX, _axisY);
                    // series.useOpenGL = true
                    dataSeries[i] = series
                    if(root.colors.length > i) {
                        series.color = root.colors[i]
                    }
                }
            }
        }
}
