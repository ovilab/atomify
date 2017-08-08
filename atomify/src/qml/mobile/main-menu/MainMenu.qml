import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
// import QtGraphicalEffects 1.0

import "qrc:/mobile/style"

Item {
    id: mainMenuRoot
    property bool revealed: true
    property var blurSource: null
    signal loadSimulation(var simulation)
    signal startIntroAnimation

    Component.onCompleted: {
        //        systemsGrid.setup()
    }

    state: revealed ? "revealed" : "hidden"

//    function startIntroAnimation() {
//        intro.startAnimation()
//    }

    function readMore(text) {
        descriptionText.text = text
        stackView.push(descriptionText)
    }

    function loadFirstSimulation() {
        //        loadSimulation(chamberSimulation)
    }

    function hide() {
        mainMenuRoot.revealed = false
    }

    //    Chamber {
    //        id: chamberSimulation
    //    }

    MouseArea {
        enabled: mainMenuRoot.revealed
        anchors.fill: parent
        onPressed: {
            mainMenuRoot.revealed = false
        }
    }

    Item {
        id: systemsViewRectangle
        enabled: revealed // Keeps button presses from being caught by internal MouseAreas

        anchors {
            fill: parent
            //            margins: Math.min(parent.width, parent.height) * 0.05
        }

        //        border.width: 1.0
        //        border.color: Qt.rgba(0.5, 0.5, 0.5, 0.9)
//        color: Qt.rgba(0.05, 0.05, 0.05, 1.0)

        MouseArea {
            // Dummy mouse area to avoid interacting with underlying simulation
            enabled: mainMenuRoot.revealed
            anchors.fill: parent
        }

//        FastBlur {
//            anchors.fill: parent
//            source: blurSource
//            radius: 32
//        }

        Rectangle {
            anchors.fill: parent
            color: Qt.rgba(0.05, 0.05, 0.05, 0.6)
        }

        StackView {
            id: stackView
            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
                bottom: parent.bottom

//                margins: Math.min(parent.width, parent.height) * 0.05
            }
            initialItem: mainMenu
            delegate: StackViewDelegate {
                pushTransition: StackViewTransition {
                    PropertyAnimation {
                        target: enterItem
                        property: "y"
                        from: enterItem.height
                        to: 0
                        duration: 600
                        easing.type: Easing.InOutQuad
                    }
                    PropertyAnimation {
                        target: exitItem
                        property: "y"
                        from: 0
                        to: -exitItem.height
                        duration: 600
                        easing.type: Easing.InOutQuad
                    }
                }
                popTransition: StackViewTransition {
                    PropertyAnimation {
                        target: enterItem
                        property: "y"
                        from: -enterItem.height
                        to: 0
                        duration: 600
                        easing.type: Easing.InOutQuad
                    }
                    PropertyAnimation {
                        target: exitItem
                        property: "y"
                        from: 0
                        to: exitItem.height
                        duration: 600
                        easing.type: Easing.InOutQuad
                    }
                }
            }
        }

        Image {
            id: backButton
            anchors {
                top: parent.top
                left: parent.left
                margins: Style.baseMargin
            }

            width: Style.touchableSize
            height: width
            source: "qrc:/images/back.png"
            enabled: stackView.depth > 1
            opacity: stackView.depth > 1

            rotation: 90

            Behavior on opacity {
                NumberAnimation {
                    duration: 1000
                    easing.type: Easing.InOutQuad
                }
            }

            MouseArea {
                anchors.fill: parent
                onPressed: {
                    if(stackView.depth > 1) {
                        stackView.pop()
                    }
                }
            }
        }
    }

    Component {
        id: mainMenu

        Item {
            id: mainMenuRoot2
            property string title: "Atomify"

            width: parent.width
            height: parent.height

            Column {
                anchors {
                    left: parent.left
                    leftMargin: Style.touchableSize
                    verticalCenter: parent.verticalCenter
                }
                width: mainMenuRoot2.width * 0.4
                spacing: mainMenuRoot2.width * 0.02
//                height: mainMenuRoot.width * 0.4
                MainMenuButton {
                    height: mainMenuRoot2.width * 0.07
                    width: parent.width
                    text: "Continue simulation"
                    onClicked: {
                        hide()
                    }
                }
                MainMenuButton {
                    height: mainMenuRoot2.width * 0.07
                    width: parent.width
                    text: "Select simulation"
                    onClicked: {
                        stackView.push(simulationSelectionView)
                    }
                }
                MainMenuButton {
                    height: mainMenuRoot2.width * 0.07
                    width: parent.width
                    text: "About"
                    onClicked: {
                        stackView.push(aboutView)
                    }
                }
            }

//            Intro {
//                id: intro
//                anchors.fill: parent
//                state: "started"
//            }
        }
    }

//    Component {
//        id: packageSelectionView

//        PackageSelectionView {

//        }
//    }

    SimulationsView {
        id: simulationSelectionView
        visible: false
        onLoadSimulation: {
            mainMenuRoot.loadSimulation(simulation)
            stackView.pop(0)
        }
        onReadMore: {
            mainMenuRoot.readMore(text)
        }
    }

    About {
        id: aboutView
        visible: false
    }

    Item {
        id: descriptionText
        property alias text: descriptionTextText.text
        Text {
            id: descriptionTextText
            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
                bottom: parent.bottom
                topMargin: Style.touchableSize * 2
                leftMargin: Style.baseMargin
                rightMargin: Style.baseMargin
                bottomMargin: Style.baseMargin
            }
            color: "white"
            font.pixelSize: parent.width * 0.025
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            font.weight: Font.Light
        }
    }

    states: [
        State {
            name: "revealed"
            PropertyChanges {
                target: systemsViewRectangle
                opacity: 1.0
                scale: 1.0
            }
        },
        State {
            name: "hidden"
            PropertyChanges {
                target: systemsViewRectangle
                opacity: 0.0
                scale: 0.85
            }
        }
    ]

    transitions: [
        Transition {
            from: "revealed"
            to: "hidden"
            ParallelAnimation {
                NumberAnimation {
                    properties: "opacity"
                    duration: 350
                    easing.type: Easing.InOutQuad
                }
                NumberAnimation {
                    properties: "scale"
                    duration: 350
                    easing.type: Easing.InOutQuad
                }
            }
        },
        Transition {
            from: "hidden"
            to: "revealed"
            ParallelAnimation {
                NumberAnimation {
                    properties: "opacity"
                    duration: 350
                    easing.type: Easing.InOutQuad
                }
                NumberAnimation {
                    properties: "scale"
                    duration: 200
                    easing.type: Easing.InOutQuad
                }
            }
        }

    ]
}
