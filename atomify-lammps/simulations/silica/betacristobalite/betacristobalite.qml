import QtQuick 2.5
import "qrc:/core"
import "qrc:/mobile/dashboard/controls"
import "qrc:/mobile/dashboard"

Simulation {
    id: root
    name: "Silica beta cristobalite"
    description: "
<p>
Silica is stable in many different crystal structures.
It is also known as silicon dioxide with the chemical formula SiO<sub>2</sub>.
</p>
<p>
In this simulation, we explore a beta cristobalite structure,
which is a mineral found in acidic volcanic rocks and
converted diatomaceous deposits.
</p>

<p><b>What to look for</b></p>

<p>
You will be able to see the corner-bonded SiO<sub>4</sub> tetrahedra
that are characteristic for this structure.
</p>
"
    longDescription: "Try adjusting the temperature to above 1700 Kelvin which is the melting point. Do you see the crystal structure disappear?"
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
        Pressure {
            xLabel: "Time [fs]"
            xScale: 1000
            xRange: 1000
        },
        Thermostat {
            minimumValue: 10
            maximumValue: 2500
            temperatureDampening: 0.2
        }
    ]
}
