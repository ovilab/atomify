import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import Atomify 1.0
import "qrc:/core"
import "qrc:/mobile/dashboard/controls"

Simulation {
    name: "Maxwell's demon"
    description: "<p>In this simulation we have two boxes that particles can be in. We have a vibrating plate at the bottom making the particles collide and jump.</p>
<p>The second law of thermodynamics says that entropy tends to increase (I like to add 'except when it doesn't). This would mean that the particles are evenly distributed in the left and the right box.</p>
<p>In this simulation we will see that the entropy actually decreases, where all particles end up being on the same side.</p>
<p>This does not violate the second law of thermodynamics since we add energy to the system through the bottom plate.</p>
"
    initialCameraPosition: Qt.vector3d(20,0,0)
    initialUpVector: Qt.vector3d(0,0,-1)
    initialViewCenter: Qt.vector3d(0,0,-10)
    controllers: [
        AtomPosition {

        }

    ]
}

