import QtQuick 2.4
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import Atomify 1.0
import SimVis 1.0

Item {
    id: sliceRoot
//    property Slice slice
    width: parent.width
    height: columnLayout.height
    property real labelWidth: 90
    ColumnLayout {
        id: columnLayout
        spacing: 10

        Row {
            spacing: 2
            Label {
                width: sliceRoot.labelWidth
                text: "Distance: "
            }

            SpinBox {
                id: distance
                decimals: 1
                minimumValue: 0
                maximumValue: 200
                stepSize: 1
                value: slice ? slice.distance : 1
                onValueChanged: {
                    slice.distance = value
                }
            }
        }

        Row {
            spacing: 2
            Label {
                width: sliceRoot.labelWidth
                text: "Origo (x,y,z): "
            }

            SpinBox {
                minimumValue: 0
                stepSize: 1
                value: slice ? slice.origo.x : 0
                onValueChanged: {
                    var x = parseFloat(value)
                    var y = parseFloat(slice.origo.y)
                    var z = parseFloat(slice.origo.z)
                    slice.origo = Qt.vector3d(x,y,z)
                }
            }
            SpinBox {
                minimumValue: 0
                stepSize: 1
                value: slice ? slice.origo.y : 0
                onValueChanged: {
                    var x = parseFloat(slice.origo.x)
                    var y = parseFloat(value)
                    var z = parseFloat(slice.origo.z)
                    slice.origo = Qt.vector3d(x,y,z)
                }
            }
            SpinBox {
                minimumValue: 0
                stepSize: 1
                value: slice ? slice.origo.z : 0
                onValueChanged: {
                    var x = parseFloat(slice.origo.x)
                    var y = parseFloat(slice.origo.y)
                    var z = parseFloat(value)
                    slice.origo = Qt.vector3d(x,y,z)
                }
            }
        }

        Row {
            spacing: 2
            Label {
                width: sliceRoot.labelWidth
                text: "Normal (x,y,z): "
            }

            SpinBox {
                minimumValue: 0
                stepSize: 1
                value: slice ? slice.normal.x : 0
                onValueChanged: {
                    var x = parseFloat(value)
                    var y = parseFloat(slice.normal.y)
                    var z = parseFloat(slice.normal.z)
                    slice.normal = Qt.vector3d(x,y,z)
                }
            }
            SpinBox {
                minimumValue: 0
                stepSize: 1
                value: slice ? slice.normal.y : 0
                onValueChanged: {
                    var x = parseFloat(slice.normal.x)
                    var y = parseFloat(value)
                    var z = parseFloat(slice.normal.z)
                    slice.normal = Qt.vector3d(x,y,z)
                }
            }
            SpinBox {
                minimumValue: 0
                stepSize: 1
                value: slice ? slice.normal.z : 0
                onValueChanged: {
                    var x = parseFloat(slice.normal.x)
                    var y = parseFloat(slice.normal.y)
                    var z = parseFloat(value)
                    slice.normal = Qt.vector3d(x,y,z)
                }
            }
        }

        Row {
            spacing: 2
            Label {
                width: sliceRoot.labelWidth
                text: "Slice width: "
            }

            SpinBox {
                id: sliceWidth
                decimals: 1
                minimumValue: 0
                maximumValue: 200
                stepSize: 1
                value: slice ? slice.width : 1
                onValueChanged: {
                    slice.width = value
                }
            }
        }
    }
}

