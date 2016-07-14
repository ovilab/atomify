import QtQuick 2.5
import "qrc:/core"
import "qrc:/mobile/dashboard/controls"
import "qrc:/mobile/dashboard"

Simulation {
    name: "Silica"
    controllers: [
        Thermometer {
            timeLabel: "t [fs]"
            timeScale: 1000
            timeRange: 0.5
        },
        Thermostat {}
    ]
}

