pragma Singleton
import QtQuick 2.5

Item {
    id: styleRoot
    property real windowWidth
    property real windowHeight
    property real minimumTouchableSize: Math.max(windowWidth, windowHeight) / 25
    property real maximumTouchableSize: Math.max(windowWidth, windowHeight) / 10
    property real pixelDensity: 72
    property real touchableSize: 6 * baseSize
    property real baseSize: pixelDensity / 10
    property real baseMargin: 4 * baseSize

    property alias font: fontObject
    property int renderType: Text.QtRendering

    Item {
        id: fontObject
        property alias heading: headingObject
        property alias button: buttonObject

        property real size: 2.5 * baseSize
        property color color: "#dedede"

        Item {
            id: headingObject
            property color color: Qt.rgba(0.4, 0.4, 0.4, 1.0)
            property real size: 2 * font.size
        }

        Item {
            id: buttonObject
            property color color: "#dedede"
            property real size: 1.4 * font.size
        }
    }

    function reset(width, height, screen) {
        styleRoot.windowWidth = width
        styleRoot.windowHeight = height
        styleRoot.pixelDensity = screen.pixelDensity

        if(Qt.platform.os === "android" || Qt.platform.os === "ios") {
            if(pixelDensity === 0) {
                console.warn("Style.reset(): Pixel density is zero. Assuming 72 dpi.")
                pixelDensity = 72
            }
        } else {
            if(pixelDensity < 50) {
                styleRoot.pixelDensity = Math.max(windowWidth, windowHeight) * 0.1
            }
        }
    }
}
