import QtQuick 2.0

import "qrc:/mobile/style"

Item {
    id: mainMenuRoot

    signal continueClicked
    signal simulationsClicked
    property bool revealed: state !== "hidden"

    state: revealed ? "" : "hidden"

    states: [
        State {
            name: "hidden"
            PropertyChanges {
                target: mainMenuRoot
                anchors.leftMargin: -contents.width
            }
        }
    ]

    transitions: [
        Transition {
            NumberAnimation {
                target: mainMenuRoot
                property: "anchors.leftMargin"
                duration: 360
                easing.type: Easing.InOutQuad
            }
        }
    ]

    width: parent.width
    height: parent.height

    anchors {
        leftMargin: 0
        left: parent.left
        top: parent.top
        bottom: parent.bottom
    }

    Rectangle {
        id: contents
        anchors {
            left: parent.left
            top: parent.top
            bottom: parent.bottom
        }

        color: "#333"
        width: Math.min(parent.width, parent.height) * 0.85

        Column {
            anchors {
                fill: parent
                margins: Style.baseMargin
            }
            MainMenuLeftButton {
                text: "Continue"
                onClicked: {
                    revealed = false
                    continueClicked()
                }
            }
            MainMenuLeftButton {
                text: "Simulations"
                onClicked: {
                    revealed = false
                    simulationsClicked()
                }
            }
            MainMenuLeftButton {
                text: "About"
            }
        }
    }

    MouseArea {
        anchors {
            left: contents.right
            right: parent.right
            top: parent.top
            bottom: parent.bottom
        }
        propagateComposedEvents: true
        onClicked: {
            if(revealed) {
                revealed = false
            } else {
                mouse.accepted = false
            }
        }
    }
}

