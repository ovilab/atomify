import QtQuick 2.6
import QtCharts 2.1
import Atomify 1.0
import "mobile/dashboard/controls"

Item {
    property alias compute: rdf
    property var simulatorControls: [
        rdf
    ]

    width: 400
    height: 400

    ChartView {
        id: miniChart
        anchors.fill: parent
        antialiasing: true
        legend.visible: false
        title: "Pair distribution function"
        // backgroundColor: "transparent"
        titleColor: "white"

        CPLineSeries {
            axisX: _axisX
            axisY: _axisY
            dataSource: DataSource {
                id: dataSource0
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
    }
}
