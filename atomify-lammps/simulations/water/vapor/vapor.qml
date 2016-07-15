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
    controllers: [
        Thermometer {
            timeLabel: "t [fs]"
            timeScale: 1000
            timeRange: 0.5
        },
        Thermostat {}
    ]
}

