import QtQuick 2.5
import QtQuick.Layouts 1.1

import "qrc:/visualization"

Item {
    id: controlRoot
    signal clicked

    property list<Item> simulatorControls
    property Component fullControl
    property Item miniControl
    property list<Component> components
    property string name: "Untitled"
    property real itemSize: 150
    property bool active: true
}

