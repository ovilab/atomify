import QtQuick 2.7
import QtQuick.Controls 1.4

import "."

MenuItem {
    id: root
    property string name
    Component.onCompleted: {
        EventCenter.register(root)
    }
    Component.onDestruction: {
        EventCenter.unregister(root)
    }
}
