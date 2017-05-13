import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import Atomify 1.0
import "qrc:/core"
import "qrc:/mobile/dashboard/controls"

Simulation {
    id: root
    name: "SSZ-13 ReaxFF"
    description: "<p>One of the most complex force fields is called ReaxFF. It is very heavy for the computer, but there is no reason why it should not run on your phone/tablet.</p>
<p>The structure here is another zeolite called SSZ-13 (or CHA) and has one of the silicon sites replaced with Aluminum.</p>"
    initialCameraPosition: Qt.vector3d(0,0,50)
    initialUpVector: Qt.vector3d(0,-1,0)
    controllers: [
        Summary {
            system: root.system
        },
        Thermometer {
            xRange: 250
            xLabel: "Time [fs]"
            yLabel: "Temperature [K]"
        }
    ]
}

