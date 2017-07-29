import QtQuick 2.5
import "qrc:/core"
import "qrc:/mobile/dashboard/controls"
import "qrc:/mobile/dashboard"

Simulation {
    id: root
    name: "SiC gas"
    description: "
<p>
Gas of randomly placed silicon and carbon atoms.
</p>

<p><b>What to look for</b></p>

<p>
If you look carefully, you can see the atomic configuration going towards a diamond crystal. What happens if you change the temperature?
</p>
"
    initialCameraPosition: Qt.vector3d(0,0,46)
    orthographic: false
    controllers: [
        Summary {
            system: root.system
        },
        Thermometer {
            xLabel: "Time [fs]"
            xScale: 1000
            xRange: 1000
        }
    ]
}
