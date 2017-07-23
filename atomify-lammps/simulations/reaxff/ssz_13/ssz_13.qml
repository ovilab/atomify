import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import Atomify 1.0
import "qrc:/core"
import "qrc:/mobile/dashboard/controls"

Simulation {
    id: root
    name: "Zeolite SSZ-13"
    description: "<p>This structure is a zeolite called SSZ-13 (or CHA) and has one of the silicon sites replaced with Aluminum and a hydrogen atom.</p>
    <p>This is a very heavy simulation, so unless your phone/tablet is very new, you must zoom and carefully look for movement.</p>"
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

