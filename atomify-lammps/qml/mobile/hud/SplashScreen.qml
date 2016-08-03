import QtQuick 2.5
import QtQuick.Controls 1.4

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
        color: Qt.rgba(1.0, 1.0, 1.0, 0.75)

        Image {
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.margins: 10
            width: parent.width*0.15
            height: width / 1.47058824
            source: "qrc:/images/watermolecule.png"
        }

        Text {
            x: 10
            y: 10
            width: parent.width - 20
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            horizontalAlignment: Text.AlignHCenter
//            font.pixelSize: Style.font.size
//            color: Style.font.color
            textFormat: Text.RichText
            text:
"
<h1>Welcome to Atomify</h1>
<br><br>
<p>We simulate how atoms and molecules behave with a physical model called molecular dynamics. It uses Newton's laws to move atoms around.</p>
<p>You will be able to study gases, liquids and solids and see different properties and effects. </p>
"
        }

        Label {
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            anchors.margins: 10
            text: "Tap anywhere to close"
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
