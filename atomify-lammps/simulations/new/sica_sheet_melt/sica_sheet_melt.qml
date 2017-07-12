import QtQuick 2.5
import "qrc:/core"
import "qrc:/mobile/dashboard/controls"
import "qrc:/mobile/dashboard"

Simulation {
    id: root
    name: "SiC nanosheet melting"
    description: "
<p>
Silicon carbide nanosheets is a new exciting material like graphene. It is a very stable sheet with high melting temperature. The structure does not get destroyed even at 2000 kelvins. It does, of course, melt eventually. 
</p>

<p><b>What to look for</b></p>

<p>
At initial temperature T=2000K, it is not melting. It does, however, have a lot of defects that appears and disappears quickly. Also look at the system from the side. It looks flat from the top, but from the side we can clearly see the bulky surface. Try increasing the temperature to see what happens when it melts.
</p>
"
    initialCameraPosition: Qt.vector3d(0,0,75)
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
