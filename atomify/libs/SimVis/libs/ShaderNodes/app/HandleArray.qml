import QtQuick 2.0

Item {
    id: root

    property string name
    property int count

    height: column.height

    Column {
        id: column
        Repeater {
            model: root.count
            Plug {
                name: root.name
                index: model.index
            }
        }
    }

    Text {
        anchors {
            verticalCenter: column.verticalCenter
        }
        text: name
    }
}
