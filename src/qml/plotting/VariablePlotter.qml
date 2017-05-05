import QtQuick 2.7
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.0
import QtQuick.Window 2.2
import Atomify 1.0
import QtCharts 2.1
import QtQuick.Controls.Styles 1.4

Window {
    id: root
    property var dataSeries
    property Variable variable
    width: 500
    height: 500
    onVariableChanged: {
        if(!variable) return
        updateSeries("line")
        variable.data.updated.connect(updateGraph)
        variable.data.xySeries = dataSeries
        variable.data.updateXYSeries(dataSeries)
        title = "Variable '"+variable.identifier+"'"
        variable.willBeDestroyed.connect(function() {
            variable = null
            timer.stop()
            root.close()
        })
    }

    onVisibleChanged: {
        if(!visible) {
            if(!variable) return
            variable.data.updated.disconnect(updateGraph)
        } else {
            updateLimits()
        }
    }

    function updateGraph() {
        if(!root.visible) return;
        variable.data.updateXYSeries(dataSeries)
    }

    function updateLimits() {
        var xMin = 1e9
        var xMax = -1e9
        var yMin = 1e9
        var yMax = -1e9

        var data = variable.data
        data.updateLimits()
        xMin = Math.min(xMin, data.xMin)
        xMax = Math.max(xMax, data.xMax)
        yMin = Math.min(yMin, data.yMin)
        yMax = Math.max(yMax, data.yMax)

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

        if (type === "line") {
            dataSeries = chart.createSeries(ChartView.SeriesTypeLine, variable.identifier, _axisX, _axisY);
        } else {
            dataSeries = chart.createSeries(ChartView.SeriesTypeScatter, variable.identifier, _axisX, _axisY);
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
                title: variable ? variable.identifier : ""

                ValueAxis {
                    id: _axisX
                    tickCount: 3
                    min: 0
                    max: 100
                    titleText: "Time"
                    color: "white"
                    labelsColor: "gray"
                }

                ValueAxis {
                    id: _axisY
                    tickCount: 3
                    min: 0
                    max: 1000
                    titleText: "Value"
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
                    onClicked: root.variable.data.clear()
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
