import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import QtQuick.Window 2.2
import Atomify 1.0
import QtCharts 2.1
import QtQuick.Controls.Styles 1.4

Window {
    id: root
    property Compute compute
    property Variable variable
    width: 500
    height: 500
    onComputeChanged: {
        if(!compute) return
        compute.data1D["histogram"].xySeries = series
        compute.willBeDestroyed.connect(function() {
            compute = null
            timer.stop()
            root.close()
        })
    }

    onVariableChanged: {
        if(!variable) return
        variable.data.xySeries = series
        variable.willBeDestroyed.connect(function() {
            variable = null
            timer.stop()
            root.close()
        })
    }

    onVisibleChanged: {
        if(!visible) {
            if(compute) compute.data1D["histogram"].updatedHistogram.disconnect(updateGraph)
            if(variable) variable.data.updatedHistogram.disconnect(updateGraph)
        }
    }

    function updateGraph(data) {
        if(!root.visible) return;
        data.updateHistogram(series, plotSeries)
        axisX.min = data.xMin
        axisX.max = data.xMax
        axisY.min = data.yMin
        axisY.max = data.yMax
    }

    Timer {
        id: timer
        interval: 250
        repeat: true
        running: true
        onTriggered: {
            if(compute) updateGraph(compute.data1D["histogram"])
            if(variable) updateGraph(variable.data)
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
                legend.visible: false
                titleColor: "black"
                title: {
                    if(compute) return compute.identifier
                    if(variable) return variable.identifier
                    return ""
                }

                AreaSeries {
                    color: "#00357F"
                    borderColor: "#FEFEFE"
                    borderWidth: 1
                    axisX: ValueAxis {
                        id: axisX
                        min: 0
                        max: 1
                    }
                    axisY: ValueAxis {
                        id: axisY
                        min: 0
                        max: 1
                    }

                    upperSeries: LineSeries {
                        id: series
                    }
                }

                Rectangle {
                    color: "white"
                    width: chart.width*0.15
                    height: chart.height*0.3
                    radius: 5
                    ChartView {
                        width: parent.width
                        height: parent.height+50
                        margins {
                            bottom: 0
                            top: 0
                            left: 0
                            right: 0
                        }

                        theme: ChartView.ChartThemeLight
                        backgroundColor: Qt.rgba(1.0, 1.0, 1.0, 0.0)
                        legend.visible: false
                        antialiasing: true

                        BoxPlotSeries {
                            id: plotSeries
                            boxWidth: 1.0
                            axisX: BarCategoryAxis { visible: false }
                            axisY: BarCategoryAxis { visible: false }

                            BoxSet { id: boxSet }
                        }
                    }
                }
            }
            Row {
                Layout.fillWidth: true
                leftPadding: 12
                spacing: 5
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
