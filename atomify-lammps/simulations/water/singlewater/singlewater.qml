import QtQuick 2.5
import "qrc:/core"
import "qrc:/mobile/dashboard/controls"

Simulation {
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
}

