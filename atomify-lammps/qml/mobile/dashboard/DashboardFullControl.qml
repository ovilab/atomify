import QtQuick 2.5
import QtQuick.Layouts 1.1

import "qrc:/mobile/style"
import "qrc:/visualization"

Item {
    id: grid

    default property alias stuff: layout.children
    property AtomifyVisualizer visualizer


//    width: 100
//    height: 100

    GridLayout {
        id: layout
        columns: width > height ? 2 : 1
        anchors {
            fill: parent
            margins: Style.baseMargin
        }
    }
}

