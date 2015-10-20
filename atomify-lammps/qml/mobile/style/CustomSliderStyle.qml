import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.1
import QtQuick.Window 2.0
import "../style" 1.0

SliderStyle {
    id: sliderStyle
    property string handleLabel: ""
    property bool activated: true

    handle: Rectangle {
        implicitWidth: Style.touchableSize
        implicitHeight: implicitWidth
        radius: width
        border.width: width * 0.1
        border.color: "#f7fbff"
//        color: "#deebf7"
        color: sliderStyle.activated ? "#deebf7" : "#ccc"

        Text {
            anchors.centerIn: parent
            font.pixelSize: parent.height * 0.5
            text: sliderStyle.handleLabel
            color: "#444"
            rotation: control.orientation === Qt.Vertical ? 90 : 0

            renderType: Qt.platform.os === "linux" ? Text.NativeRendering : Text.QtRendering
        }
    }
    groove: Rectangle {
        id: rect
        implicitWidth: 200
        implicitHeight: 8
        color: "#c6dbef"
        radius: 8

        Rectangle {
            color: sliderStyle.activated ? "#4292c6" : "#777"
            anchors {
                bottom: parent.bottom
                top: parent.top
                left: parent.left
            }
            radius: parent.radius
            width: (control.value - control.minimumValue) / (control.maximumValue - control.minimumValue) * rect.width
        }
    }
}
