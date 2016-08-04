import QtQuick 2.5
import "qrc:/core"
import "qrc:/mobile/dashboard/controls"
import "qrc:/mobile/dashboard"

Simulation {
    // TODO reduce the size of the simulation, runs too slow
    // TODO with a reduced size, it may be possible to run fast enough for clusters to form in seconds

    name: "Water vapor"
    description: "
<p>
Here we see 3000 water molecules with an average distance of 2 nanometers
at temperature equal to 300 Kelvin.
This is water vapor at a molecular level.
</p>
<p><b>Things to look for</b></p>
<p>
Notice how water molecules can bind to each other temporarily through
hydrogen bonds before they break apart again.
Try to raise and lower the temperature to see if clusters
of water molecules form.
</p>
"
    initialCameraPosition: Qt.vector3d(21.0025, -63.4384, 40.7256)
    initialUpVector: Qt.vector3d(0.530134, 0.575353, 0.622839)
    controllers: [
        Thermometer {
            xLabel: "t [fs]"
            xScale: 1000
            xRange: 0.5
        },
        Thermostat {
            minimumValue: 10
            maximumValue: 1000
        }
    ]
}
