import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import Atomify 1.0
import QtCharts 2.0
import "qrc:/mobile/style"
import "qrc:/mobile/dashboard"
DashboardControl {
    id: root
    property string xLabel: "Time"
    property string yLabel: "Count"
    property real xRange: 0
    property real xScale: 1.0
    property real yScale: 1.0
    property var colors: []

    name: "Counter"
    fullControl: Column {
        ChartScrollerNew {
            id: miniChart
            colors: root.colors
            xRange: root.xRange
            xScale: root.xScale
            yScale: root.yScale
            xLabel: root.xLabel
            yLabel: root.yLabel
            active: root.active
            showLegend: true
            autoScrollX: true
            autoScrollY: false
            yMin: 0
            yMax: 100

            anchors {
                left: parent.left
                right: parent.right
            }
            dataSources: [ varLeft.data, varRight.data ]
            height: width * 2.5 / 4
        }
    }
    simulatorControls: [
        Variable {
            id: varLeft
            identifier: "left"
            title: "Left"
            command: "equal count(left)/atoms"
        },
        Variable {
            id: varRight
            identifier: "right"
            title: "Right"
            command: "equal count(right)/atoms"
        }
    ]
}
