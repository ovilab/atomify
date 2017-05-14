import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import Atomify 1.0
import "qrc:/core"
import "qrc:/mobile/dashboard/controls"

Simulation {
    id: root
    name: "Ammonia-Borine (AB)"
    description: "<p>Combustion of Ammonia-Borine (AB) can be simulated with ReaxFF. Two AB molecules meet and a H<sub>2</sub> molecule is formed.</p>
<p><b>Things to look for</b></p>
<p>Can you see when the first H<sub>2</sub> molecule forms?</p>
"
    initialCameraPosition: Qt.vector3d(0,0,20)
    initialUpVector: Qt.vector3d(0,-1,0)
    
    controllers: [
        Summary {
            system: root.system
        },
        Thermometer {
            xLabel: "Time [fs]"
            xRange: 250
        }
    ]
}

