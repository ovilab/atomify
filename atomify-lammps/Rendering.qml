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
            id: slicing
            checkable: true
            title: "Slicing"
            checked: renderingRoot.simulator ? renderingRoot.simulator.sliceEnabled : false
            onCheckedChanged: {
                simulator.sliceEnabled = checked
            }

            Column {
                width: 500
                height: 155
                SliceControl {
                    id: slice
                    simulator: renderingRoot.simulator
                    anchors.fill: parent
                }
            }
        }
    }
}

