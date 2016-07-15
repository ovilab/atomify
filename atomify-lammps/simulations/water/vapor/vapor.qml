import QtQuick 2.5
import "qrc:/core"
import "qrc:/mobile/dashboard/controls"
import "qrc:/mobile/dashboard"

Simulation {
    name: "Water vapor"
    description: "Here we see 3000 water molecules separated by 2 nanometer at temperature equal to 300 Kelvin."
    periodicImagesX: 1
    periodicImagesY: 1
    periodicImagesZ: 1
    initialCameraPosition: Qt.vector3d(21.0025, -63.4384, 40.7256)
    initialUpVector: Qt.vector3d(0.530134, 0.575353, 0.622839)
    controllers: [
        Thermometer {
            timeLabel: "t [fs]"
            timeScale: 1000
            timeRange: 0.5
        },
        Thermostat {}
    ]
}
