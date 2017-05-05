import QtQuick 2.6
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.4
import QtCharts 2.1
import Atomify 1.0
import "mobile/dashboard/controls"

Item {
    property alias compute: rdf
    property System system
    property var simulatorControls: [
        rdf
    ]

    width: parent.columns===1 ? parent.width : parent.width*0.5
    height: width

    ChartView {
        id: miniChart
        anchors.fill: parent
        antialiasing: true
        legend.visible: true
        title: "Pair distribution function"
        titleColor: "black"

        CPLineSeries {
            axisX: _axisX
            axisY: _axisY
            dataSource: DataSource {
                id: dataSource0
            }
        }

        CPLineSeries {
            axisX: _axisX
            axisY: _axisY
            dataSource: DataSource {
                id: dataSource1
            }
        }

        CPLineSeries {
            axisX: _axisX
            axisY: _axisY
            dataSource: DataSource {
                id: dataSource2
            }
        }

        CPLineSeries {
            axisX: _axisX
            axisY: _axisY
            dataSource: DataSource {
                id: dataSource3
            }
        }

        ValueAxis {
            id: _axisX
            tickCount: 3
            min: 0
            max: 5.5
            titleText: "r [Å]"
            color: "white"
            labelsColor: "black"
        }
        ValueAxis {
            id: _axisY
            tickCount: 3
            min: 0
            max: 5.0
            titleText: "g(r)"
            color: "white"
            labelsColor: "black"
        }
    }

    RDF {
        id: rdf
        identifier: "rdf"
        group: "all"
        atomPairs: ["1", "2", "1", "1", "2", "2"]
        dataSource: dataSource0
        dataSource1: dataSource1
        dataSource2: dataSource2
        dataSource3: dataSource3
    }
}
