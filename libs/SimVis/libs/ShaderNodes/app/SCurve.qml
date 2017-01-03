import QtQuick 2.0
import QtQuick.Controls.Material 2.0

BezierCurve {
    id: sCurve
    controlPoint1: Qt.point(startPoint.x + 50, startPoint.y)
    controlPoint2: Qt.point(endPoint.x - 50, endPoint.y)
}
