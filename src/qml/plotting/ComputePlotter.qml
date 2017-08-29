import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.0
import QtQuick.Window 2.2
import Atomify 1.0
import QtCharts 2.1
import QtQuick.Dialogs 1.2
import QtQuick.Controls.Styles 1.4

WindowGL2 {
    id: root
    property var dataSeries: []
    property Compute compute
    width: 500
    height: 500
    onComputeChanged: {
        if(!compute) return
        updateSeries("line")
        for(var key in compute.data1D) {
            var data = compute.data1D[key]
            data.updated.connect(updateGraphs(key))

            data.updateXYSeries(dataSeries[key])
            data.xySeries = dataSeries[key]
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
        if(!ylimLock.checked) {
            _axisY.min = (yMin>0) ? 0.95*yMin : 1.05*yMin
            _axisY.max = (yMax<0) ? 0.95*yMax : 1.05*yMax
            _axisY.applyNiceNumbers()
        }

        _axisY.tickCount = ticks.value
        if(roundXAxis.checked) {
            _axisX.applyNiceNumbers()
            _axisY.tickCount = ticks.value
        }
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
                var label = key
                if(compute.data1D[key].label !=="") {
                    label = compute.data1D[key].label
                }
                var series = chart.createSeries(ChartView.SeriesTypeLine, label, _axisX, _axisY);

                series.useOpenGL = true
                dataSeries[key] = series
            }
        } else {
            for(key in compute.data1D) {
                label = key
                if(compute.data1D[key].label !=="") {
                    label = compute.data1D[key].label
                }
                series = chart.createSeries(ChartView.SeriesTypeScatter, label, _axisX, _axisY);
                series.useOpenGL = true
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
                    tickCount: ticks.value

                    min: 0
                    max: 100
                    titleText: compute ? compute.xLabel : ""
                    color: "white"
                    labelsColor: "gray"
                }

                ValueAxis {
                    id: _axisY
                    tickCount: 8
                    min: 0
                    max: 1000
                    titleText: compute ? compute.yLabel : ""
                    color: "white"
                    labelsColor: "gray"
                }
            }
            Column {
                Layout.fillWidth: true
                leftPadding: 12
                spacing: 5
                Row {
                    spacing: 10
                    Button {
                        id: clearButton
                        width: exportButton.width
                        text: "Clear"
                        onClicked: root.compute.clear()
                    }
                    CheckBox {
                        id: roundXAxis
                        text: "Round x-axis"
                    }
                    CheckBox {
                        id: ylimLock
                        text: "Lock ylim"
                    }
                }
                Row {
                    spacing: 10
                    Button {
                        id: exportButton
                        height: clearButton.height
                        text: "Export"
                        onClicked: menu.open()
                        Menu {
                            id: menu
                            y: exportButton.height

                            MenuItem {
                                text: "Text file"
                                onClicked: {
                                    // TODO: pause simulation
                                    fileDialog.mode = "text"
                                    fileDialog.open()
                                }
                            }
                            MenuItem {
                                text: "MATLAB"
                                onClicked: {
                                    // TODO: pause simulation
                                    fileDialog.mode = "matlab"
                                    fileDialog.open()
                                }
                            }
                            MenuItem {
                                text: "Python"
                                onClicked: {
                                    // TODO: pause simulation
                                    fileDialog.mode = "python"
                                    fileDialog.open()
                                }
                            }
                        }
                    }

                    Label {
                        height: exportButton.height
                        verticalAlignment: Text.AlignVCenter
                        text: "# ticks: "
                    }

                    Slider {
                        id: ticks
                        height: exportButton.height
                        from: 3
                        to: 9
                        stepSize: 1
                        snapMode: Slider.SnapAlways
                        value: 5
                    }
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

    FileDialog {
        id: fileDialog
        selectExisting : false
        property string mode
        title: "Please choose a location to save"
        nameFilters: {
            if(mode==="text") {
                return [ "Text files (*.txt)" ]
            } else if(mode==="matlab") {
                return [ "MATLAB files (*.m)" ]
            } else if(mode==="python") {
                return [ "Python files (*.py)" ]
            } else return [""]
        }

        onAccepted: {
            if(mode==="text") {
                compute.exportToTextFile(fileDialog.fileUrl)
            } else if(mode==="matlab") {
                compute.exportToMatlabFile(fileDialog.fileUrl)
            } else if(mode==="python") {
                compute.exportToPythonFile(fileDialog.fileUrl)
            }
        }
    }
}
