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

    height: textField.height
    Label {
        anchors {
            left: parent.left
            right: textField.left
            verticalCenter: parent.verticalCenter
        }
        wrapMode: Label.WrapAtWordBoundaryOrAnywhere
        text: handle.name
    }

    Button {
        anchors {
            right: textField.left
        }
        text: "Reset"
        onClicked: handle.reset()
    }

    TextField {
        id: textField
        anchors {
            right: parent.right
            verticalCenter: parent.verticalCenter
        }
        text: handle.value
        inputMethodHints: Qt.ImhPreferNumbers
        onEditingFinished: {
            handle.value = parseFloat(textField.text)
        }
    }
}
