import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import Atomify 1.0
import "qrc:/core"
import "qrc:/mobile/dashboard"

Simulation {
    name: "Diffusion"

    description: "In gases and solids, there is net movement from regions of high concentration " +
                 "to regions of low concentration. In this simulation, a group of red particles " +
                 "will diffuse into the blue solution with time."

    longDescription: "Play with the thermostat and see how the diffusion process is slowed when " +
                     "lowering the temperature and sped up when raising the temperature."

    controllers: [
        TemperatureControl {},
        DashboardControl{
            miniControl: DashboardMiniControl {
                Rectangle {
                    anchors.fill: parent
                    color: "red"
                }
            }
            fullControl: DashboardFullControl {
                Rectangle {
                    color: "blue"
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }
                Rectangle {
                    color: "green"
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }
                Rectangle {
                    color: "red"
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }
            }
        }
    ]
}

