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
    property real xRange: 0
    property string xLabel: "t [ps]"
    property string yLabel: "&lt;r<sup>2</sup>(t)&gt; [Å<sup>2</sup>]"
    property real xScale: 1.0
    property real yScale: 1.0
    property var msdCompute
    property var vacfCompute
    // property alias frequency: diffusion_vacf.frequency

    fullControl: Column {
        ChartScrollerNew {
            id: miniChart
            title: "Diffusion coefficient"
            xRange: root.xRange
            xScale: root.xScale
            yScale: root.yScale
            xLabel: root.xLabel
            yLabel: root.yLabel
            active: root.active
            autoScroll: true

            anchors {
                left: parent.left
                right: parent.right
            }

            // dataSources: [diffusion_msd.data, diffusion_vacf.data]
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
            dependencies: [msdCompute]
        },
        Fix {
            id: fix
            identifier: "vacf_fix"
            enabled: vacfCompute !== undefined
            command: vacfCompute !== undefined ? " all vector 1 c_"+vacfCompute.identifier+"[4]" : ""
            dependencies: vacfCompute !== undefined ? [vacfCompute] : []
        }/*,
        Variable {
            id: diffusion_vacf
            frequency: 1
            enabled: vacfCompute !== undefined
            identifier: "diffusion_vacf"
            command: vacfCompute !== undefined ? "equal 0.3333333*dt*trap(f_"+fix.identifier+")" : ""
            dependencies: [fix]
        }*/
    ]
}
