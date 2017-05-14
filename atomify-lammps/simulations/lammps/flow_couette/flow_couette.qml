import QtQuick 2.5
import "qrc:/core"

Simulation {
    name: "Couette flow"
    description: "<p>Couette flow happens in a liquid between two walls if the walls don't move with the same velocity. In this simulation, the upper wall is given a velocity.</p>
<p><b>Things to look for</b></p>
<p>After equlibrium is reached, the atoms near the walls almost have the same velocity as the walls.</p>
"
    initialCameraPosition: Qt.vector3d(0,0,22)
    initialUpVector: Qt.vector3d(0,-1,0)
}

