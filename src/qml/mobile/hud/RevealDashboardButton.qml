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
import "../hud"

Item {
    id: revealDashboardButton
    signal clicked
    property bool revealed: true
    anchors {
        right: parent.right
        bottom: parent.bottom
    }
    enabled: revealed
    state: revealed ? "revealed" : "hidden"
    width: Style.touchableSize * 1.5
    height: width
    
    states: [
        State {
            name: "hidden"
            PropertyChanges {
                target: revealDashboardButton
                opacity: 0.0
            }
        },
        State {
            name: "revealed"
            PropertyChanges {
                target: revealDashboardButton
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
        smooth: true
        
        source: "qrc:/images/dashboard-plot.png"
    }
    
    MouseArea {
        anchors.fill: parent
        onClicked: {
            revealDashboardButton.clicked()
        }
    }
}
