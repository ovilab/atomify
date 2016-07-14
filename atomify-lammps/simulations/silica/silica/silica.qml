import QtQuick 2.5
import "qrc:/core"
import "qrc:/mobile/dashboard/controls"

Simulation {
    name: "Silica"
    controllers: [
        Thermometer {},
        Thermostat {}
    ]
}

