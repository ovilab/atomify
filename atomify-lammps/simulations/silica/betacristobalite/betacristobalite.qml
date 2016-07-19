import QtQuick 2.5
import "qrc:/core"
import "qrc:/mobile/dashboard/controls"
import "qrc:/mobile/dashboard"

Simulation {
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
    controllers: [
        Thermometer {
            timeLabel: "t [fs]"
            timeScale: 1000
            timeRange: 0.5
        },
        Thermostat {
            minimumValue: 10
            maximumValue: 2500
        }
    ]
}

