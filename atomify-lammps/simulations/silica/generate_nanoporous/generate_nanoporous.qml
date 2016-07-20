import QtQuick 2.5
import "qrc:/core"
import "qrc:/mobile/dashboard/controls"

Simulation {
    name: "Nanoporous glass"
    description: "
<p>
In this simulation we show how to generate a
nanoporous silica structure (amorphous glass)
by regulating the temperature.
</p>

<p><b>What is happening?</b></p>

<p>
We first heat the system to 4500 Kelvin before we expand the system to a lower density.
We can the quickly lower the temperature (quench the system) to reach a nice nanoporous glass structure.
</p>
"
    initialCameraPosition: Qt.vector3d(51.0378, -39.4625, 55.4313)
    initialUpVector: Qt.vector3d(0.286427, 0.885236, 0.366486)
    controllers: [
        // TODO Thermometer not working in this simulation. Wrong timescale?
//        Thermometer {
//            timeLabel: "t [fs]"
//            timeScale: 1000
//            timeRange: 0.5
//        }
    ]
}

