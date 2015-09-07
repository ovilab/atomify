import QtQuick 2.3
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import MySimulator 1.0

Item {
    id: renderingRoot
    property MySimulator simulator

    ColumnLayout {
        spacing: 10

        GroupBox {
            checkable: true
            title: "Slicing"
            checked: false
            onCheckedChanged: {
                renderingRoot.simulator.sliceEnabled = checked
            }

            CheckBox {
                id: slicingEnabled
                visible: false
            }
            Column {
                width: 500
                height: 155
                Slice {
                    id: slice
                    simulator: renderingRoot.simulator
                    anchors.fill: parent
                }
            }
        }
    }
}

