import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Window 2.0
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0
import Atomify 1.0
import SimVis 1.0
import Qt.labs.settings 1.0

import "../../visualization"
import "../main-menu"
import "../dashboard"
import "../style"

Item {
    id: revealSimulationsViewButton
    property bool revealed: true
    signal clicked

    width: Style.touchableSize * 1.5
    height: width

    enabled: revealed
    state: revealed ? "revealed" : "hidden"

    states: [
        State {
            name: "hidden"
            PropertyChanges {
                target: revealSimulationsViewButton
                opacity: 0.0
            }
        },
        State {
            name: "revealed"
            PropertyChanges {
                target: revealSimulationsViewButton
                opacity: 1.0
            }
        }
    ]

    transitions: [
        Transition {
            NumberAnimation {
                properties: "opacity"
                duration: 200
                easing.type: Easing.InOutQuad
            }
        }
    ]

    Image {
        anchors {
            fill: parent
            margins: parent.width * 0.2
        }
        source: "qrc:/images/ic_info_white_48dp.png"
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            revealSimulationsViewButton.clicked()
        }
    }
}
