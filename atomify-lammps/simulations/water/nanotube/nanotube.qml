import QtQuick 2.5
import "qrc:/core"
import "qrc:/mobile/dashboard/controls"

Simulation {
    id: root
    name: "Water through nanotube"
    description: "<p>This simulation shows water flow through a carbon nanotube under extreme pressure.</p>" +
                 "<p><b>What to look for?</b></p>" +
                 "<p>Notice how the H<sub>2</sub>O form a crystal structure (ice).</p>"
    initialCameraPosition: Qt.vector3d(-42.5741, -5.20759, -12.4129)
    initialUpVector: Qt.vector3d(0, 1.0, 0)
    controllers: [
        Summary {
            system: root.system
        },
        Gravity {
            minimumValue: -1
            maximumValue: 1.0
            initialValue: 0.1
            enabled: true
        }

    ]
}

