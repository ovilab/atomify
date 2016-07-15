import QtQuick 2.5
import "qrc:/core"
import "qrc:/mobile/dashboard/controls"

Simulation {
    name: "Generating nanoporous glass"
    description: "In this simulation we show how to generate a nanoporous silica structure (amorphous glass)."
    longDescription: "We first heat the system to 4500 Kelvin before we expand the system to a lower density."+
                     "We can the quickly lower the temperature (quench the system) to reach a nice nanoporous glass structure."
    controllers: [
        Thermometer {
            timeLabel: "t [fs]"
            timeScale: 1000
            timeRange: 0.5
        }
    ]
}

