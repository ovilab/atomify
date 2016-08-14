import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import Atomify 1.0
import "qrc:/core"
import "qrc:/mobile/dashboard/controls"

Simulation {
    id: root
    name: "Diffusion coefficient"
    description: "<p>The diffusion coefficient can be measured as the slope of the mean square displacement r^2(t) (MSD).</p>
<p>The MSD is defined as the average distance atoms have to their original position a time t earlier.</p>
<p><b>Things to look for</b></p>
<p>In the beginning of the MSD measurement, on a short time scale, the atoms move more or less in a straight line giving rise to the non-linear ballistic regime.
The system then goes over to the linear regime where we can read out the diffusion coefficient. </p>"
    initialCameraPosition: Qt.vector3d(0,0,20)
    initialUpVector: Qt.vector3d(0,-1,0)
    controllers: [
        Summary {
            system: root.system
        },
        DiffusionCoefficient {
            xScale: 341.53026 // 3.405e-10*sqrt(1.66e-27/1.65e-21) = 3.4153026e-13 seconds is t=1.0
            xRange: 1000
            // yScale: 11.594025 // 3.405^2
            // xLabel: "Time [fs]"
            msdCompute: msd.compute
            // vacfCompute: vacf.compute
            // frequency: 10
        },
        MeanSquareDisplacement {
            id: msd
            xScale: 341.53026 // 3.405e-10*sqrt(1.66e-27/1.65e-21) = 3.4153026e-13 seconds is t=1.0
            yScale: 11.594025 // 3.405^2
            // xRange: 1000
            xLabel: "Time [fs]"
        },
//        VelocityAutoCorrelationFunction {
//            id: vacf
//            xScale: 341.53026 // 3.405e-10*sqrt(1.66e-27/1.65e-21) = 3.4153026e-13 seconds is t=1.0
//            xRange: 1000
//            xLabel: "Time [fs]"
//        },
        Thermometer {
            xScale: 341.53026 // 3.405e-10*sqrt(1.66e-27/1.65e-21) = 3.4153026e-13 seconds is t=1.0
            yScale: 119.6
            xRange: 1000
            xLabel: "Time [fs]"
            yLabel: "Temperature [K]"
        },
        Thermostat {
            unitScale: 119.6
            minimumValue: 1.0
            maximumValue: 500.0
            temperatureDampening: 0.1
        }
    ]
}

