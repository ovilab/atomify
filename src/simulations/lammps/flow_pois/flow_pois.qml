import QtQuick 2.5
import "qrc:/core"
import "qrc:/mobile/dashboard/controls"
Simulation {
    id: root
    name: "Poiseuille flow"
    description: "<p>Poiseuille flow is observed when a liquid experiences a pressure drop through a pipe. In this 2D simulation, this effect is mimicked by applying a constant force on all the fluid atoms.</p>
<p><b>Things to look for</b></p>
<p>After equlibrium is reached, we see that the atoms near the wall have a much lower velocity than those in the middle. This gives the famous flow profile from the Poiseuille flow.</p>
"
    initialCameraPosition: Qt.vector3d(0,0,20)
    initialUpVector: Qt.vector3d(0,-1,0)
    controllers: [
        Summary {
            system: root.system
        }
    ]
}

