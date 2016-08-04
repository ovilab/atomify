import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import Atomify 1.0
import "qrc:/core"
import "qrc:/mobile/dashboard/controls"

Simulation {
    name: "Patterns"
    description: "<p>When granular materials (such as sand and marbles) lie on a vibrating plate, they can end up producing very cool patterns.</p>
<p>We only need to tune the vibrational frequency and amplitude.</p>"
    customColorShaderCode: "
highp float zScaled = 0.5 + 0.5* (position.z + 5.0) / 5.0;
zScaled = clamp(zScaled, 0.0, 1.0);
customColor *= zScaled;
"
    initialCameraPosition: Qt.vector3d(0,0,50)
    controllers: [ ]
}

