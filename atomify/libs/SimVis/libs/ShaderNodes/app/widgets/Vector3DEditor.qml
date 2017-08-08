import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import Qt.labs.settings 1.0
import QtGraphicalEffects 1.0
import QtQuick.Controls 1.0 as QtControls

import ShaderNodes 1.0

import ".."

Item {
    property Handle handle
    property real xValue: parseFloat(xField.text)
    property real yValue: parseFloat(yField.text)
    property real zValue: parseFloat(zField.text)

    function updateHandleValue() {
        handle.value = Qt.vector3d(xValue, yValue, zValue)
    }

    height: row.height
    Label {
        anchors {
            left: parent.left
            right: row.left
            verticalCenter: parent.verticalCenter
        }
        wrapMode: Label.WrapAtWordBoundaryOrAnywhere
        text: handle.name
    }

    Row {
        id: row
        anchors {
            right: parent.right
        }
        height: xField.height


        Button {
            text: "Reset"
            onClicked: handle.reset()
        }

        TextField {
            id: xField
            text: handle.value.x
            inputMethodHints: Qt.ImhPreferNumbers
            onEditingFinished: {
                updateHandleValue()
            }
        }

        TextField {
            id: yField
            text: handle.value.y
            inputMethodHints: Qt.ImhPreferNumbers
            onEditingFinished: {
                updateHandleValue()
            }
        }

        TextField {
            id: zField
            text: handle.value.z
            inputMethodHints: Qt.ImhPreferNumbers
            onEditingFinished: {
                updateHandleValue()
            }
        }

    }
}
