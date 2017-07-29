import QtQuick 2.5
import QtQuick.Controls 1.4
import "../style"

Item {
    id: root
    property real fillRatio: 0.9
    property real margin: 0.5*(1.0 - fillRatio)*parent.height
    property alias radius: rectangle.radius

    Rectangle {
        id: rectangle
        radius: 10
        height: parent.height*fillRatio
        width: parent.width - 2.0*margin
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.leftMargin: margin
        anchors.rightMargin: margin
        anchors.topMargin: margin
        anchors.bottomMargin: margin
        color: Qt.rgba(Style.color.background.r, Style.color.background.g, Style.color.background.b, 0.9)

        Image {
            id: image
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.margins: 10
            width: parent.width*0.15
            height: width
            source: "qrc:/images/icon.png"
        }

        Image {
            id: uioLogo
            anchors.top: parent.top
            anchors.right: parent.right
            anchors.margins: 10
            width: parent.width*0.15
            height: width
            source: "qrc:/images/uio.png"
        }

        Text {
            id: welcomeHeading
            x: parent.width*0.5 - width*0.5

            anchors {
                top: parent.top
                margins: Style.baseMargin
                topMargin: image.height*0.4
            }

//            anchors {
//                top: parent.top
//                left: image.right
//                right: parent.right
//                margins: Style.baseMargin
//                topMargin: image.height*0.5
//            }
            font.pixelSize: root.width * 0.05

            color: Style.font.color
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            text: "Welcome to Atomify"
        }

        Text {
            id: welcomeText
            anchors {
                top: image.bottom
                left: parent.left
                right: parent.right
                bottom: parent.bottom
                margins: Style.baseMargin
                topMargin: -8
            }

            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            color: Style.font.color
            font.pixelSize: root.width * 0.03
            text: "
We simulate how atoms and molecules behave with a physical model called molecular dynamics. It uses Newton's laws to move atoms around following the laws of physics. You can study gases, liquids and solids to see different properties and effects. Atomify is open source and uses LAMMPS as physics engine.
"
        }

            Text {
                id: nameText
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                anchors.margins: Style.baseMargin
                color: Style.font.color
                font.pixelSize: welcomeText.font.pixelSize
                text:
"Anders Hafreager and Svenn-Arne Dragly
PhD candidates, University of Oslo"
            }

            Image {
                id: closeImage
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                anchors.margins: 10
                width: Style.baseMargin
                height: width
                source: "qrc:/images/ic_clear_white_48dp.png"
            }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            if(root.visible) {
                root.visible = false
            } else {
                mouse.accepted = false
            }
        }
    }
}
