import QtQuick 2.5
import QtQuick.Layouts 1.1
import QtQml.Models 2.2
import Atomify 1.0

QtObject {
    property string name: "Unnamed"
    property string description: "No description"
    property string longDescription: "No description"
    property string folder: ""
    property url scriptSource: (folder && identifier) ? folder + "/" + identifier + ".in" : ""
    property url screenshotSource: (folder && identifier) ? folder + "/" + identifier + ".png" : ""
    property System system
    property real pan: -10
    property real lightAttenuation: 0
    property real tilt: -20
    property real zoom: -20
    property int periodicImagesX: 1
    property int periodicImagesY: 1
    property int periodicImagesZ: 1
    property bool orthographic: false
    property string customColorShaderCode: ""
    property vector3d initialCameraPosition: Qt.vector3d(0,0,10);
    property vector3d initialViewCenter: Qt.vector3d(0,0,0);
    property vector3d initialUpVector: Qt.vector3d(0,1,0);

    property string identifier: {
        var names = folder.split("/")
        if(folder === "") {
            return ""
        }

        return names[names.length - 1]
    }
    property var simulator
    property list<Item> controllers

    // TODO add properties:
    // camera position and range
    // light settings
}
