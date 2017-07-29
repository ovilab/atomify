import QtQuick 2.5
import "qrc:/core"
import "qrc:/mobile/dashboard/controls"
import "qrc:/mobile/dashboard"

Simulation {
    id: root
    name: "Gas hydrates"
    description: "
<p>
Gas hydrate is a solid ice-like form of water that contains gas molecules in its molecular cavities. In this simulation, we have methane inside the ice cage.
</p>
"
    initialCameraPosition: Qt.vector3d(0,0,30)
    orthographic: false
    controllers: [
        Summary {
            system: root.system
        }
    ]
}
