import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import Atomify 1.0
import QtCharts 2.0
import "qrc:/mobile/style"
import "qrc:/mobile/dashboard"
DashboardControl {
    id: root
    property real xRange: 4
    property string xLabel: "t [ps]"
    property string yLabel: "&lt;v(t)v(0)&gt;"
    property real xScale: 1.0
    property real yScale: 1.0
    property alias computeCommand: compute.command
    property alias compute: compute

    name: "VACF"
    fullControl: Column {
        ChartScrollerNew {
            id: miniChart
            title: "Velocity autocorrelation function"
            xRange: root.xRange
            xScale: root.xScale
            yScale: root.yScale
            active: root.active
            autoScroll: true

            anchors {
                left: parent.left
                right: parent.right
            }

            dataSources: [ compute.vectorValues["r2"] ]
            height: width * 2.5 / 4
        }
    }
    simulatorControls: [
        Compute {
            id: compute
            identifier: "vacf_compute"
            vectorTitles: ["x", "y", "z", "r2"]
            scalarTitle: "MSD"
            command: "all vacf"
        }//,
//        FixAverageTime {
//            // fix %1 all ave/time %2 %3 %4 c_%5 file %6.txt
//            // return {QString("fix %1 all ave/time %2 %3 %4 c_%5 file %6.txt").arg(identifier()).arg(m_nEvery).arg(m_nRepeat).arg(m_nFreq).arg(m_compute->identifier(), identifier())};
//            id: fix
//            identifier: "vac_time_ave"
//            nEvery: 1
//            nRepeat: 10
//        }

    ]
}
