import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import Atomify 1.0
import "qrc:/core"
import "qrc:/mobile/dashboard/controls"

Simulation {
    name: "Diffusion coefficient"

//    description: "In gases and solids, there is net movement from regions of high concentration " +
//                 "to regions of low concentration. In this simulation, a group of red particles " +
//                 "will diffuse into the blue solution with time."
    description: "The diffusion coefficient can be measured as the slope of the mean square displacement r^2(t) (MSD)."+
                 "The MSD is defined as the average distance atoms have to their original position a time t earlier."
    initialCameraPosition: Qt.vector3d(0,0,20)
    controllers: [
        Thermometer {},
        Thermostat {}
    ]
}

