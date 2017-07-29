import QtQuick 2.4
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.1
import Atomify 1.0
import SimVis 1.0
import "../items"

GroupBox {
    id: sliceRoot
    property SliceModifier slice
    property real labelWidth: 90
    height: slice.enabled ? undefined : label.height
    label: CheckBox {
        id: checkBox
        checked: false
        onCheckedChanged: slice.enabled = checked
        text: qsTr("Slice")
    }
    ColumnLayout {
        id: columnLayout
        visible: slice.enabled
        spacing: 10

        Label {
            width: sliceRoot.labelWidth
            text: "Distance: "
        }

        ScrollableNumberField {
            value: slice.distance
            onValueChanged: slice.distance = value
        }

        Label {
            width: sliceRoot.labelWidth
            text: "Normal (x,y,z): "
        }

        ScrollableNumberField {
            value: slice.normal.x
            onValueChanged: {
                var x = parseFloat(value)
                var y = parseFloat(slice.normal.y)
                var z = parseFloat(slice.normal.z)
                slice.normal = Qt.vector3d(x,y,z)
            }
            Component.onCompleted: value = 1
        }

        ScrollableNumberField {
            value: slice.normal.y
            onValueChanged: {
                var x = parseFloat(value)
                var y = parseFloat(slice.normal.y)
                var z = parseFloat(slice.normal.z)
                slice.normal = Qt.vector3d(x,y,z)
            }
        }

        ScrollableNumberField {
            value: slice.normal.z
            onValueChanged: {
                var x = parseFloat(slice.normal.x)
                var y = parseFloat(slice.normal.y)
                var z = parseFloat(value)
                slice.normal = Qt.vector3d(x,y,z)
            }
        }

        Label {
            width: sliceRoot.labelWidth
            text: "Slice width: "
        }

        ScrollableNumberField {
            value: slice.width
            min: 0
            onValueChanged: {
                slice.width = value
            }
        }
    }
}

