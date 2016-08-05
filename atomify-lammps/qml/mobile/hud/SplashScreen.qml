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
            height: width / 1.47058824
            source: "qrc:/images/watermolecule.png"
        }

        Text {
            id: welcomeHeading
            anchors {
                top: image.bottom
                left: parent.left
                right: parent.right
                margins: Style.baseMargin
                topMargin: Style.baseMargin * 0.5
            }
            font.pixelSize: Style.font.heading.size

            color: Style.font.color
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            text: "Welcome to Atomify LAMMPS"
        }

        Text {
            anchors {
                top: welcomeHeading.bottom
                left: parent.left
                right: parent.right
                bottom: parent.bottom
                margins: Style.baseMargin
                topMargin: Style.baseMargin * 0.5
            }

            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            color: Style.font.color
            text: "
We simulate how atoms and molecules behave with a physical model called molecular dynamics. It uses Newton's laws to move atoms around.
You will be able to study gases, liquids and solids and see different properties and effects.

Atomify uses LAMMPS as physics engine.
"
        }

        Label {
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            anchors.margins: Style.baseMargin * 0.5
            color: Style.font.color
            text: "Tap anywhere to hide this dialog"
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
