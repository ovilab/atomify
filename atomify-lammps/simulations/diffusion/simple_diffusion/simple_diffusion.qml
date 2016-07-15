import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import Atomify 1.0
import "qrc:/core"
import "qrc:/mobile/dashboard/controls"

Simulation {
    name: "Diffusion"

//    description: "In gases and solids, there is net movement from regions of high concentration " +
//                 "to regions of low concentration. In this simulation, a group of red particles " +
//                 "will diffuse into the blue solution with time."
    description: "We have colored atoms differently to illustrate how gases mixes due to diffusion. " +
                 "Think about this the next time you mix a drink."

    longDescription: "Play with the thermostat and see how the diffusion process is slowed when " +
                     "lowering the temperature and sped up when raising the temperature."

    controllers: [
//        Thermometer {},
//        Thermostat {}
    ]
}

