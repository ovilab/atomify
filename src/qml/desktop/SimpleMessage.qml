import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Window 2.0
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0
import Atomify 1.0
import SimVis 1.0
import Qt.labs.settings 1.0

import "../visualization"

Label {
    id: root
    opacity: 0
    color: "white"
    font.pixelSize: 18
    property alias animation: animation

    transitions: Transition {
        PropertyAnimation {
            id: animation
            properties: "opacity"
            easing.type: Easing.InOutQuad
            duration: 700
        }
    }

    onStateChanged: {
        if(state==="visible") {
            hideTimer.start()
        } else {
            hideTimer.stop()
        }
    }

    states: [
        State {
            name: "visible"
            PropertyChanges { target: root; opacity: 1 }
        },
        State {
            name: "hidden"
            PropertyChanges { target: root; opacity: 0 }
        }
    ]

    Timer {
        id: hideTimer
        interval: 3000
        onTriggered: {
            root.state = "hidden"
        }
    }
}
