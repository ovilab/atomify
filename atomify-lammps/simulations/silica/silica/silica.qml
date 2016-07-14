import QtQuick 2.5
import "qrc:/core"
import "qrc:/mobile/dashboard/controls"

Simulation {
    name: "Silica"
    controllers: [
        Thermometer {
            timeLabel: "t [fs]"
            timeScale: 1000
            timeRange: 0.1
        },
        Thermostat {}
    ]
}

