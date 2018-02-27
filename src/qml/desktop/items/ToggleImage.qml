import QtQuick 2.5
import QtGraphicalEffects 1.0

Item {
    property bool toggled
    property alias source: image.source
    
    Rectangle {
        anchors {
            verticalCenter: parent.verticalCenter
            left: parent.left
            right: parent.right
        }
        height: width

        // opacity: toggled ? 0.5 : 0.0
        visible: Qt.platform.os==="osxx" ? toggled : false
        color: "#3377ef"
    }

    Image {
        id: image
        anchors.fill: parent
        fillMode: Image.PreserveAspectFit
        opacity: Qt.platform.os!=="osxx" ? 1.0 : (toggled ? 1.0 : 0.5)
    }

    ColorOverlay {
        anchors.fill: parent
        source: image
        color: "#888"
        visible: Qt.platform.os==="osxx" ? false : !toggled
    }
    Glow {
        anchors.fill: parent
        visible: Qt.platform.os==="osxx" ? false : toggled
        source: image
        samples: 12
        color: "#3377ef"
    }
}
