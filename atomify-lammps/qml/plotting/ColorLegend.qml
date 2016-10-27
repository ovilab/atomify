import QtQuick 2.5
import QtQuick.Controls 2.0

Item {
    id: root
    property real min: 0
    property real max: 1

    ColorMaps {
        id: colorMaps
    }

    Rectangle {
        id: legend
        anchors.margins: 20
        anchors.bottom: parent.bottom
        anchors.top: parent.top
        anchors.right: parent.right
        border.color: "white"
        border.width: 1
        width: 50
        rotation: 180
        gradient: colorMaps.jet_
    }

    Label {
        id: legendMax
        anchors {
            top: legend.top
            right: legend.left
            rightMargin: 10
        }

        width: 30
        horizontalAlignment: Qt.AlignRight
        color: "white"
        text: root.max.toFixed(2)
    }

    Label {
        anchors {
            verticalCenter: legend.verticalCenter
            right: legend.left
            rightMargin: 10
        }
        width: legendMax.width
        horizontalAlignment: Qt.AlignRight
        color: "white"
        text: ((root.max + root.min) / 2).toFixed(2)
    }

    Label {
        anchors {
            bottom: legend.bottom
            right: legend.left
            rightMargin: 10
        }
        width: legendMax.width
        horizontalAlignment: Qt.AlignRight
        color: "white"
        text: root.min.toFixed(2)
    }
}
