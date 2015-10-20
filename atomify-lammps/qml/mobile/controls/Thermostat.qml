import QtQuick 2.0
import QtQuick.Controls 1.1
import "../style"

Rectangle {
    id: dialRoot
    property bool activated: false
    property real value: 0.0
    property real secondaryValue: 620.0
    property real minimumValue: 0.0
    property real maximumValue: 1.0
    property real _valueNormalized: (value - minimumValue) / (maximumValue - minimumValue)
    property real _secondaryValueNormalized: (secondaryValue - minimumValue) / (maximumValue - minimumValue)
    property real _secondaryValueLastUpdate: Date.now()
    color: "black"
//    width: 64
//    height: width
    radius: width / 2
    border.width: width * 0.01
    border.color: "white"

    onValueChanged: {
        if(value > maximumValue) {
            value = maximumValue
        }
        if(value < minimumValue) {
            value = minimumValue
        }
    }

    onSecondaryValueChanged: {
        var currentTime = Date.now()
        var timeDiff = currentTime - _secondaryValueLastUpdate
        if(timeDiff <= 0 || timeDiff > 1000) {
            return
        }

        secondaryValueAnimation.duration = timeDiff * 1.1
        _secondaryValueLastUpdate = currentTime
    }

//    Rectangle {
//        id: innerCircle
//        anchors.centerIn: parent
//        width: parent.width * 0.7
//        height: width
//        radius: width / 2

//        color: dialRoot.activated ? "#2171b5" : "#777"
//    }

    Image {
        id: innerCircle
        anchors.centerIn: parent
        width: parent.width * 0.7
        height: width

        source: dialRoot.activated ? "qrc:/images/button_enabled.png" : "qrc:/images/button_disabled.png"
    }

    Text {
        id: thermostatText
        anchors.centerIn: parent
        text: (dialRoot.value).toFixed(0)
        color: "white"
        font.pixelSize: parent.height * 0.25

        renderType: Qt.platform.os === "linux" ? Text.NativeRendering : Text.QtRendering
    }

    Text {
        anchors {
            bottom: thermostatText.top
            bottomMargin: -height / 4
            horizontalCenter: thermostatText.horizontalCenter
        }

        font.pixelSize: thermostatText.font.pixelSize * 0.6
        color: "white"
        opacity: 0.7
        text: "°C"

        renderType: Qt.platform.os === "linux" ? Text.NativeRendering : Text.QtRendering
    }

    Text {
        anchors {
            top: thermostatText.bottom
            horizontalCenter: parent.horizontalCenter
        }

        font.pixelSize: parent.height * 0.10
        color: dialRoot.activated ? "#2FFF1C" : "#ccc"

        text: dialRoot.activated ? "ON" : "OFF"

        renderType: Qt.platform.os === "linux" ? Text.NativeRendering : Text.QtRendering

    }

//    Switch {
//        anchors {
//            top: thermostatText.bottom
//            horizontalCenter: parent.horizontalCenter
//        }

//        height: parent.height * 0.10
//        checked: dialRoot.activated

//    }

//    Row {
//        anchors {
//            top: thermostatText.bottom
//            topMargin: height * 0.5
//            horizontalCenter: parent.horizontalCenter
//        }
//        spacing: dialRoot.width * 0.05
//        Rectangle {
//            id: greenCircle
//            width: dialRoot.width * 0.03
//            height: width
//            radius: width / 2
//            color: dialRoot.activated ? "#2FFF1C" : "#39663B"
//        }
//        Rectangle {
//            id: redCircle
//            width: greenCircle.width
//            height: width
//            radius: width / 2
//            color: dialRoot.activated ? "#702C2C" : "#FF3D3D"
//        }
//    }


    PathInterpolator {
        id: motionPath2

        property real centerX: dialRoot.width / 2
        property real centerY: dialRoot.height / 2
        property real radius: dialRoot.width / 2 - indicator.height / 2 - dialRoot.border.width
        property real startAngle: 4*Math.PI/3
        property real stopAngle: 5*Math.PI/3

        progress: dialRoot._secondaryValueNormalized

        path: Path {
            startX: motionPath.centerX + Math.cos(motionPath.startAngle) * motionPath.radius
            startY: motionPath.centerY - Math.sin(motionPath.startAngle) * motionPath.radius

            PathArc {
                x: motionPath.centerX + Math.cos(motionPath.stopAngle) * motionPath.radius
                y: motionPath.centerY - Math.sin(motionPath.stopAngle) * motionPath.radius
                radiusX: motionPath.radius
                radiusY: motionPath.radius
                useLargeArc: true
            }
        }

        Behavior on progress {
            NumberAnimation {
                id: secondaryValueAnimation
                duration: 200
            }
        }
    }

    Rectangle {
        id: indicator2
        width: dialRoot.width * 0.04; height: dialRoot.width * 0.07
        color: dialRoot.secondaryValue > dialRoot.maximumValue ? "#e31a1c" : "#a6cee3"

        //bind our attributes to follow the path as progress changes
        x: motionPath2.x - width / 2
        y: motionPath2.y - height / 2

        rotation: motionPath2.angle
    }

    PathInterpolator {
        id: motionPath

        property real centerX: dialRoot.width / 2
        property real centerY: dialRoot.height / 2
        property real radius: dialRoot.width / 2 - indicator.height / 2 - dialRoot.border.width
        property real startAngle: 4*Math.PI/3
        property real stopAngle: 5*Math.PI/3

        progress: dialRoot._valueNormalized

        path: Path {
            startX: motionPath.centerX + Math.cos(motionPath.startAngle) * motionPath.radius
            startY: motionPath.centerY - Math.sin(motionPath.startAngle) * motionPath.radius

            PathArc {
                x: motionPath.centerX + Math.cos(motionPath.stopAngle) * motionPath.radius
                y: motionPath.centerY - Math.sin(motionPath.stopAngle) * motionPath.radius
                radiusX: motionPath.radius
                radiusY: motionPath.radius
                useLargeArc: true
            }
        }
    }

    Rectangle {
        id: indicator
        width: dialRoot.width * 0.04; height: dialRoot.width * 0.10
        color: "white"

        //bind our attributes to follow the path as progress changes
        x: motionPath.x - width / 2
        y: motionPath.y - height / 2

        rotation: motionPath.angle
    }

    MouseArea {
        anchors {
            fill: parent
        }
        property point startPosition
        property bool changeStarted: false
        onPressed: {
            startPosition = Qt.point(mouse.x, mouse.y)
            changeStarted = false
        }
        onPositionChanged: {
            //                var dx = mouse.x - previousPosition.x
            //                var dy = mouse.y - previousPosition.y
            //                if(mouse.x < dialRoot.width / 2) {
            //                    dialRoot.value += dx - dy
            //                } else {
            //                    dialRoot.value += dx + dy
            //                }

            //                previousPosition = Qt.point(mouse.x, mouse.y)

            var deltaX = mouse.x - startPosition.x
            var deltaY = mouse.y - startPosition.y
            var dr2 = deltaX*deltaX + deltaY*deltaY
            var changeThreshold = Style.touchableSize
            if(dr2 > changeThreshold*changeThreshold) {
                changeStarted = true
            }

            if(!changeStarted) {
                return
            }

            var centerX = dialRoot.width / 2
            var centerY = dialRoot.height / 2
            var posX = mouse.x - centerX
            var posY = mouse.y - centerY
            var angle = (-Math.atan2(-posY, posX) + 4*Math.PI - Math.PI / 3) % (2*Math.PI) + Math.PI/3
            var value = (angle - 2*Math.PI/3) / (2*Math.PI - Math.PI/3) * (dialRoot.maximumValue - dialRoot.minimumValue) + dialRoot.minimumValue
            if(value < dialRoot.minimumValue) {
                if(posX > 0) {
                    value = dialRoot.maximumValue
                } else {
                    value = dialRoot.minimumValue
                }
            }
            dialRoot.value = value
        }
        onReleased: {
            if(!changeStarted) {
                dialRoot.activated = !dialRoot.activated
            }
        }
    }
}
