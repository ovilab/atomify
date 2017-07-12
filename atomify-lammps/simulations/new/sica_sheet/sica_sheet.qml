import QtQuick 2.5
import "qrc:/core"
import "qrc:/mobile/dashboard/controls"
import "qrc:/mobile/dashboard"

Simulation {
    id: root
    name: "Curly SiC nanosheet"
    description: "
<p>
Silicon carbide nanosheets is a new exciting material like graphene. If it has free ends and not on a substrate, it will curl into a ball.
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
        },
        Thermostat {
            minimumValue: 10
            maximumValue: 3000
            temperatureDampening: 0.2
        }
    ]
}
