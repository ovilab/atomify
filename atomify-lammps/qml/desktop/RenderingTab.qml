import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import AtomifySimulator 1.0
import SimVis 1.0

import "../visualization"

Item {
    id: renderingRoot
    property AtomifyVisualizer atomifyVisualizer

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
                atomStyle: atomifyVisualizer.simulator.atomStyle
            }
        }

        GroupBox {
            id: slicing
            height: sliceControl.height + 20
            width: parent.width
            checkable: true
            title: "Slicing"
            checked: atomifyVisualizer ? atomifyVisualizer.slice.enabled : false

            onCheckedChanged: {
                atomifyVisualizer.slice.enabled = checked
            }

            SliceControl {
                id: sliceControl
                slice: atomifyVisualizer.slice
            }
        }

        GroupBox {
            id: light
            height: lightControl.height + 20
            width: parent.width
            title: "Light"

            LightControl {
                id: lightControl
                light: atomifyVisualizer.light
            }
        }

        CheckBox {
            id: ortho
            text: "Orthographic camera"
            onCheckedChanged: {
                atomifyVisualizer.camera.setOrthographic(checked)
            }
        }

        CheckBox {
            onCheckedChanged: {
                atomifyVisualizer.addPeriodicCopies = checked
            }
            text: "Periodic copies"
        }
    }
}

