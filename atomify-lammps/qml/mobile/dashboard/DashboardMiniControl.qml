import QtQuick 2.5
import QtQuick.Layouts 1.1

import "qrc:/visualization"

Item {
    id: controlRoot

    signal clicked(var index)

    property int menuIndex: 0

    anchors.fill: parent
}

