import QtQuick 2.5
import "qrc:/core"
import "qrc:/mobile/dashboard/controls"
import "qrc:/mobile/dashboard"

Simulation {
    id: root
    name: "Zeolite ZSM-5"
    description: "A group of silicon and oxygen atoms can form different forms of zeolites. Zeolites are nanoporous structures with lots of pores (channels) where flow can occur."
    initialCameraPosition: Qt.vector3d(-73.7451, 3.78433, -2.67165)
    initialUpVector: Qt.vector3d(0.049758, 0.997954, 0.0401179)
    orthographic: false
    controllers: [
        Summary {
            system: root.system
        },
        Thermometer {
            xLabel: "t [fs]"
            xScale: 1000
            xRange: 1000
        },
        Pressure {
            xLabel: "t [fs]"
            xScale: 1000
            xRange: 1000
        },
        Thermostat {
            minimumValue: 10
            maximumValue: 2000
            temperatureDampening: 0.2
        }
    ]
}

