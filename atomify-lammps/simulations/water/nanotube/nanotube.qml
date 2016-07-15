import QtQuick 2.5
import "qrc:/core"
import "qrc:/mobile/dashboard/controls"

Simulation {
    name: "Water through nanotube"
    description: "This simulation shows water flow through a carbon nanotube under extreme pressure."
    longDescription: "Notice how the H<sub>2</sub>O form a crystal structure (ice)."
    controllers: [
        Thermometer {
            timeLabel: "t [fs]"
            timeScale: 1000
            timeRange: 0.5
        }
    ]
}

