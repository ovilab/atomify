pragma Singleton
import QtQuick 2.5
import QtCharts 2.0

Item {
    id: styleRoot
    property real windowWidth
    property real windowHeight
    property real minimumTouchableSize: Math.max(windowWidth, windowHeight) / 25
    property real maximumTouchableSize: Math.max(windowWidth, windowHeight) / 10
    property real pixelDensity: 72
    property real touchableSize: 6 * baseSize
    property real baseSize: pixelDensity
    property real baseMargin: 4 * baseSize
    property real spacing: 2 * baseSize
    property alias color: colorsObject
    property alias border: borderObject

    property alias font: fontObject
    property int renderType: Text.QtRendering
    property QtObject chart: QtObject {
        property color backgroundColor: "transparent"
    }

    QtObject {
        id: colorsObject
        property color background: "#444444"
        property color foreground: "#888888"
        property alias border: borderObject.color
    }


    QtObject {
        id: borderObject
        property color color: "#9ecae1"
        property color lightColor: "#d5e8f2"
        property real width: 2.0
}

    Item {
        id: fontObject
        property alias heading: headingObject
        property alias button: buttonObject

        property real size: 2.5 * baseSize
        property color color: "#dedede"

        Item {
            id: headingObject
            property color color: Qt.rgba(0.5, 0.5, 0.5, 1.0)
            property real size: 2 * font.size
        }

        Item {
            id: buttonObject
            property color color: "#dedede"
            property real size: 1.4 * font.size
        }
    }

    function reset(width, height, screen) {
        console.log("Resetting style")
        styleRoot.windowWidth = width
        styleRoot.windowHeight = height
        styleRoot.pixelDensity = screen.pixelDensity
        console.log("Got width " + width + " height " + height + " pixel density " + screen.pixelDensity)

        if(Qt.platform.os === "android" || Qt.platform.os === "ios") {
            if(pixelDensity === 0) {
                console.warn("Style.reset(): Pixel density is zero. Assuming pixel density.")
                pixelDensity = 6.0
            }
        }
        if(pixelDensity < 5.0) {
            console.warn("Pixel density reported as too small. Using width or height.")
            styleRoot.pixelDensity = Math.max(windowWidth, windowHeight) * 0.01
        }
        console.log("Done resetting style")
    }
}
