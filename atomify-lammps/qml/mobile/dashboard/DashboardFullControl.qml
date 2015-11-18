import QtQuick 2.5
import QtQuick.Layouts 1.1

import "qrc:/mobile/style"
import "qrc:/visualization"

Item {
    id: grid

    default property alias stuff: layout.children
    property AtomifyVisualizer visualizer

    width: parent ? parent.itemWidth : 100
    height: parent ? parent.itemHeight : 100

    GridLayout {
        id: layout
        columns: width > height ? 2 : 1
        anchors {
            fill: parent
            margins: Style.baseMargin
        }
    }
}

