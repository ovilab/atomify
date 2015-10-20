import QtQuick 2.5

Item {
    property real maximumValue: 100
    property real minimumValue: -100
    property string title: ""
    property real scientificLimit: 9999
    width: 100
    height: 62

    function inScientificLimit(value) {
        return ((value >= -scientificLimit && value <= -1 / scientificLimit) ||
                (value >= 1 / scientificLimit && value <= scientificLimit)) || value === 0
    }

    function formatValue(value) {
        if(inScientificLimit(value)) {
            if(value > 1 || value < -1) {
                return value.toFixed(0)
            } else {
                return value.toPrecision(1)
            }
        } else {
            return value.toExponential(0)
        }
    }

    Text {
        anchors {
            left: parent.left
            top: parent.top
            margins: parent.height * 0.04
        }
        font.pixelSize: 12
        text: formatValue(maximumValue)
        color: "white"

        renderType: Qt.platform.os === "linux" ? Text.NativeRendering : Text.QtRendering
    }

    Text {
        anchors {
            left: parent.left
            bottom: parent.bottom
            margins: parent.height * 0.04
        }
        font.pixelSize: 12
        text: formatValue(minimumValue)
        color: "white"

        renderType: Qt.platform.os === "linux" ? Text.NativeRendering : Text.QtRendering
    }

    Text {
        anchors {
            right: parent.right
            top: parent.top
            margins: parent.height * 0.04
        }
        font.pixelSize: 12
        text: title
        color: "white"

        renderType: Qt.platform.os === "linux" ? Text.NativeRendering : Text.QtRendering
    }
}
