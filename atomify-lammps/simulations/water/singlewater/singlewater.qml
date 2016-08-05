import QtQuick 2.5
import Atomify 1.0
import "qrc:/core"
import "qrc:/mobile/dashboard/controls"

Simulation {
    id: root
    name: "Water molecule"
    description: "
<p>In this simulation, a single water molecule jiggles around on its own.
There are no external forces, making the molecule only subject to
its internal bonds.</p>
<p><b>Things to look for</b></p>
<p>You can observe that there is a preferred angle between the hydrogen
atoms and the oxygen atoms.
Also, see if you can find the different vibrational modes:
The H<sub>2</sub>O molecule has a symmetric and antisymmetric stretch of the
OH bonds and a scissoring bend of the H-O-H angle.</p>
"

    initialCameraPosition: Qt.vector3d(0,0,2.64575)
    controllers: [
        Summary {
            system: root.system
        },
        Angle {
            atom1: 1
            atom2: 2
            atom3: 3
            xRange: 100.0
            xScale: 1000
            xLabel: "t [fs]"
            title: "The experimental value is about 105 degrees."
        }
    ]
}
