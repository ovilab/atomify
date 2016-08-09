import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Window 2.0
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0
import Atomify 1.0
import SimVis 1.0
import Qt.labs.settings 1.0
import QtCharts  2.0
import "mobile"
import "mobile/style"
import "desktop"

ApplicationWindow {
    id: applicationRoot

    visible: true

    property string mode: {
//        if(["android", "ios", "winphone"].indexOf(Qt.platform.os)) {
//            return "mobile"
//        }
        return "desktop"
    }

    Settings {
        id: settings
//        property alias mode: applicationRoot.mode
        property alias width: applicationRoot.width
        property alias height: applicationRoot.height
    }

    function resetStyle() {
        console.log("Resetting style")
//        width = Math.max(width, 1650)
//        height = Math.max(height, 1080)
        Style.reset(width, height, Screen)
    }

    onWidthChanged: {
        resetStyle()
        console.log("Width changed")
    }

    onHeightChanged: {
        console.log("Height changed")
        resetStyle()
    }

    Component.onCompleted: {
        // resetStyle()
    }

    MainDesktop {
        visible: mode === "desktop"
        anchors.fill: parent
    }

//    MainMobile {
//        visible: mode === "mobile"
//        anchors.fill: parent
//    }

//    Shortcut {
//        sequence: StandardKey.AddTab
//        context: Qt.ApplicationShortcut
//        onActivated: {
//            if(mode === "desktop") {
//                mode = "mobile"
//            } else {
//                mode = "desktop"
//                //                tempPlot.xMax = mySimulator.simulationTime
//                //                tempPlot.xMin = mySimulator.simulationTime-1
//                //                maxValue = Math.max(maxValue, value)
//                //                tempPlot.yMax = maxValue
//            }
//        }
//    }

    Shortcut {
        sequence: StandardKey.FullScreen
        context: Qt.ApplicationShortcut
        onActivated: {
            if(visibility === Window.FullScreen) {
                visibility = Window.AutomaticVisibility
            } else {
                visibility = Window.FullScreen
            }
        }
    }
}
