import QtQuick 2.0

Item {
    id: resizeRectangle

    property var target: parent
    property bool forceSquare: false

    anchors.fill: target

    ResizeHandle {
        id: topLeft
        target: resizeRectangle.target
        anchors {
            left: parent.left
            top: parent.top
            leftMargin: -width / 2
            topMargin: -height / 2
        }
        xFactor: 1.0
        yFactor: 1.0
        widthFactor: -1.0
        heightFactor: -1.0
        forceSquare: resizeRectangle.forceSquare
    }

    ResizeHandle {
        id: bottomLeft
        target: resizeRectangle.target
        anchors {
            left: parent.left
            bottom: parent.bottom
            leftMargin: -width / 2
            bottomMargin: -height / 2
        }
        xFactor: 1.0
        yFactor: 0.0
        widthFactor: -1.0
        heightFactor: 1.0
        forceSquare: resizeRectangle.forceSquare
    }

    ResizeHandle {
        id: topRight
        anchors {
            right: parent.right
            top: parent.top
            rightMargin: -width / 2
            topMargin: -height / 2
        }
        target: resizeRectangle.target
        xFactor: 0.0
        yFactor: 1.0
        widthFactor: 1.0
        heightFactor: -1.0
        forceSquare: resizeRectangle.forceSquare
    }

    ResizeHandle {
        id: bottomRight
        anchors {
            right: parent.right
            bottom: parent.bottom
            rightMargin: -width / 2
            bottomMargin: -height / 2
        }
        target: resizeRectangle.target
        xFactor: 0.0
        yFactor: 0.0
        widthFactor:  1.0
        heightFactor: 1.0
        forceSquare: resizeRectangle.forceSquare
    }
}
