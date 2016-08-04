import QtQuick 2.5
import "qrc:/core"
import "qrc:/mobile/dashboard/controls"

Simulation {
    name: "Nanoporous glass"
    description: "
<p>
In this simulation we show how to generate a
nanoporous silica structure (amorphous glass)
by regulating the temperature and volume.
</p>

<p><b>What is happening?</b></p>

<p>
We first heat the system to 4500 Kelvin before we expand the system to a lower density.
We can then quickly decrease the temperature (quench the system) to reach a nice, nanoporous glass structure.
</p>
"
    initialCameraPosition: Qt.vector3d(66, -31, 85)
    initialUpVector: Qt.vector3d(0, 1.0, 0)
    initialViewCenter: Qt.vector3d(21,21,21)
    orthographic: false
    periodicImagesX: 2
    periodicImagesY: 2
    periodicImagesZ: 2

    controllers: [
        Thermometer {
            xLabel: "Time [fs]"
            xScale: 1000
        },
        Volume {
            xLabel: "Time [fs]"
            xScale: 1000
        }
    ]
}

