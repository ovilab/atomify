import QtQuick 2.5
import "qrc:/core"
import "qrc:/mobile/dashboard/controls"
import "qrc:/mobile/dashboard"

Simulation {
    name: "Zeolite ZSM-5"
    description: "A group of silicon and oxygen atoms can form different forms of zeolites. Zeolites are nanoporous structures with lots of pores (channels) where flow can occur."
    initialCameraPosition: Qt.vector3d(-73.7451, 3.78433, -2.67165)
    initialUpVector: Qt.vector3d(0.049758, 0.997954, 0.0401179)
    controllers: [
        Thermometer {
            timeLabel: "t [fs]"
            timeScale: 1000
            timeRange: 0.5
        },
        Thermostat {}
    ]
}

