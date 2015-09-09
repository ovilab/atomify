import QtQuick 2.4
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import MySimulator 1.0

Item {
    id: renderingRoot
    property MySimulator simulator

    ColumnLayout {
        spacing: 10
        x: 10
        width: parent.width - 20

        GroupBox {
            id: atomColoring
            height: atomStyleControl.height + 20
            width: parent.width
            title: "Atom style"

            AtomStyleControl {
                id: atomStyleControl
                atomStyle: simulator.atomStyle
            }
        }

        GroupBox {
            id: slicing
            height: sliceControl.height + 20
            width: parent.width
            checkable: true
            title: "Slicing"
            checked: renderingRoot.simulator ? renderingRoot.simulator.sliceEnabled : false

            onCheckedChanged: {
                simulator.sliceEnabled = checked
            }


            SliceControl {
                id: sliceControl
                simulator: renderingRoot.simulator
            }
        }
    }
}

