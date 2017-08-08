import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2
import QtGraphicalEffects 1.0

Item {
    id: root

    signal clicked

    property alias text: titleLabel.text
    property alias subtext: subtitleLabel.text

    width: 300
    height: 80

    Rectangle {
        id: rectangle
        anchors.fill: parent
        color: "#41CD52"

        Item {
            anchors {
                fill: parent
                margins: 16
            }

            Label {
                id: titleLabel
                anchors {
                    left: parent.left
                    right: parent.right
                    top: subtext.length > 0 ? parent.top : undefined
                    verticalCenter: subtext.length > 0 ? undefined : parent.verticalCenter
                }
                wrapMode: Label.WrapAtWordBoundaryOrAnywhere
                horizontalAlignment: Label.AlignHCenter
                color: "white"
                font.pixelSize: 18
            }

            Label {
                id: subtitleLabel
                anchors {
                    left: parent.left
                    right: parent.right
                    bottom: parent.bottom
                }
                wrapMode: Label.WrapAtWordBoundaryOrAnywhere
                color: "#eee"
                horizontalAlignment: Label.AlignHCenter
            }

        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                root.clicked()
            }
        }
    }

    DropShadow {
        anchors.fill: rectangle
        horizontalOffset: 6
        verticalOffset: 6
        radius: 8
        samples: 17
        color: "#22000000"

        source: rectangle
    }
}
