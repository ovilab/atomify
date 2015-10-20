pragma Singleton
import QtQuick 2.5

Item {
    id: styleRoot
    property real windowWidth
    property real windowHeight
    property real minimumTouchableSize: windowWidth / 25
    property real maximumTouchableSize: windowWidth / 10
    property real pixelDensity: 72
    property real touchableSize: 6 * baseSize
    property real baseSize: 72
    property real baseMargin: 4 * baseSize

    property alias font: fontObject

    Item {
        id: fontObject
        property alias heading: headingObject
        property alias button: buttonObject

        property real size: 2 * baseSize
        property color color: "#dedede"

        Item {
            id: headingObject
            property color color: Qt.rgba(0.4, 0.4, 0.4, 1.0)
            property real size: 2.5 * font.size
        }

        Item {
            id: buttonObject
            property color color: "#dedede"
            property real size: 1.6 * font.size
        }
    }

    function reset(width, height, pixelDensity) {
        console.log("Resetting style")

        styleRoot.windowWidth = width
        styleRoot.windowHeight = height
        styleRoot.pixelDensity = pixelDensity

        if(Qt.platform.os === "android" || Qt.platform.os === "ios") {
            if(pixelDensity === 0) {
                console.warn("Style.reset(): Pixel density is zero. Assuming 72 dpi.")
                pixelDensity = 72
            }

            baseSize = pixelDensity
        } else {
            baseSize = styleRoot.windowWidth * 0.01
        }
    }
}
