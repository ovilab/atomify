import QtQuick 2.5
import "qrc:/core"
import "qrc:/mobile/dashboard/controls"
import "qrc:/mobile/dashboard"

Simulation {
    name: "Silica beta cristobalite"
    description: "Silica is stable in many different crystal structures. Here we see a beta cristobalite structure."
    longDescription: "Try adjusting the temperature to above 1700 Kelvin which is the melting point. Do you see the crystal structure disappear?"
    controllers: [
        Thermometer {
            timeLabel: "t [fs]"
            timeScale: 1000
            timeRange: 0.5
        },
        Thermostat {}
    ]
}

