import QtQuick 2.9
import QtQuick.Controls 2.2

TextField {
    id: root
    property var value
    property real min: Number.NEGATIVE_INFINITY
    property real max: Number.POSITIVE_INFINITY
    property int decimals: 1
    selectByMouse: true

    onValueChanged: {
        if(activeFocus) {
            return
        } else {
            text = root.value.toFixed(root.decimals)
        }
    }

//    validator: DoubleValidator {

//    }
    onTextChanged: {
        if(isNaN(parseFloat(text))) {
            return;
        }

        var newValue = parseFloat(text)
        if(newValue < root.min) newValue = root.min
        else if(newValue > root.max) newValue = root.max

        root.value = newValue
    }
    MouseArea {
        anchors.fill: parent
        onWheel: {
            var speed = wheel.angleDelta.y/720
            var newValue = root.value + (Math.abs(root.value)+1)*speed
            if(newValue < root.min) newValue = root.min
            else if(newValue > root.max) newValue = root.max

            root.value = newValue
            text = root.value.toFixed(root.decimals)
        }
        hoverEnabled: false
        acceptedButtons: Qt.NoButton
        onClicked: {
            mouse.accepted = false
        }
    }
}
