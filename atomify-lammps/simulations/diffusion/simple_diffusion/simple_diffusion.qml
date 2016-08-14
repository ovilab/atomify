import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import Atomify 1.0
import "qrc:/core"
import "qrc:/mobile/dashboard/controls"

Simulation {
    id: root
    name: "Diffusion"

    description: "
<p>Watch how a liquid mixes due to diffusion.
The white particles in the middle will move randomly and mix with the red particles.
After some time, you'll see that it is completely mixed.</p>

<p>Think about this the next time you mix a drink.</p>

<p><b>A versatile simulation</b></p>

<p>The forces between the particles in this simulation are
modeled with a Lennard-Jones potential.
This is typically a good model for noble gases and other
neutral atoms and molecules.
This is in other words a good simulations for diffusion with
many different types of particles.</p>
"

    longDescription: "Play with the thermostat and see how the diffusion process is slowed when " +
                     "lowering the temperature and sped up when raising the temperature."
    initialCameraPosition: Qt.vector3d(0,0,50)
    controllers: [
        Summary {
            system: root.system
        },
        Thermometer {
            xScale: 341.53026 // 3.405e-10*sqrt(1.66e-27/1.65e-21) = 3.4153026e-13 seconds is t=1.0
            yScale: 119.6
            xLabel: "Time [fs]"
            yLabel: "Temperature [K]"
            xRange: 250
        },
        Thermostat {
            unitScale: 119.6
            minimumValue: 1.0
            maximumValue: 500.0
            temperatureDampening: 0.1
        }
    ]
}

