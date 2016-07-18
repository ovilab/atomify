import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import Atomify 1.0
import "qrc:/core"
import "qrc:/mobile/dashboard/controls"

Simulation {
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
    initialCameraPosition: Qt.vector3d(0,0,20)
    controllers: [
        Thermometer {
            timeLabel: "t [fs]"
            timeScale: 117
            timeRange: 0.5
        },
        Thermostat {}
    ]
}

