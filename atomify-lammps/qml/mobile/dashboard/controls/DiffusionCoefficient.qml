import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import Atomify 1.0
import QtCharts 2.0
import "qrc:/mobile/style"
import "qrc:/mobile/dashboard"
DashboardControl {
    id: root
    name: "Diffusion coeff"
    property real timeRange: 4
    property string xLabel: "t [ps]"
    property string yLabel: "&lt;r<sup>2</sup>(t)&gt; [Å<sup>2</sup>]"
    property real xScale: 1.0
    property real yScale: 1.0
    property var msdCompute
    property var vacfCompute
    property real time: msdCompute!==undefined ? msdCompute.time : 0.0

    fullControl: Column {
        ChartScrollerNew {
            id: miniChart
            title: "Diffusion coefficient"

            anchors {
                left: parent.left
                right: parent.right
            }

            dataSources: [diffusion_msd.data]

            height: width * 2.5 / 4
        }
    }
    simulatorControls: [
        Variable {
            id: diffusion_msd
            frequency: 1
            enabled: (msdCompute !== undefined)
            identifier: "diffusion_msd"
            command: "equal c_"+msdCompute.identifier+"[3]/6/(step*dt+1.0e-6)"
            // command: "equal pe"
            dependencies: [msdCompute]
        }
    ]
}
