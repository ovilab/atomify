import QtQuick 2.7

import "."

Shortcut {
    id: root

    property string name

    context: Qt.ApplicationShortcut

    Component.onCompleted: {
        EventCenter.register(root)
    }
    Component.onDestruction: {
        EventCenter.unregister(root)
    }

    onActivated: EventCenter.postEvent(name)
}
