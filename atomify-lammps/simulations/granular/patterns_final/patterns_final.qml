import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import Atomify 1.0
import "qrc:/core"
import "qrc:/mobile/dashboard/controls"

Simulation {
    name: "Patterns final"
    description: "<p>Same as the system called 'Patterns', but thermalized so you can see the final pattern without waiting.</p>"
    initialCameraPosition: Qt.vector3d(0,0,50)
    customColorShaderCode:
"
highp float zScaled = 0.2 + 0.8* (position.z + 7.5) / 5.0;
zScaled = clamp(zScaled, 0.0, 1.0);
customColor *= zScaled;
"
    controllers: [ ]
}

