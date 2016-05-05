import QtQuick 2.5
import QtQuick.Controls 1.4
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

Rectangle {
    id: applicationRoot
    property alias mySimulator: mainDesktop.simulator

    width: 300
    height: 480
    visible: true

    property string mode: {
        if(["android", "ios", "winphone"].indexOf(Qt.platform.os)) {
            return "mobile"
        }
        return "desktop"
    }

    Settings {
        id: settings
        property alias mode: applicationRoot.mode
        property alias width: applicationRoot.width
        property alias height: applicationRoot.height
    }

    function resetStyle() {
        Style.reset(width, height, Screen)
    }

    onWidthChanged: {
        resetStyle()
    }

    onHeightChanged: {
        resetStyle()
    }

    Component.onCompleted: {
        resetStyle()
    }

    MainDesktop {
        id: mainDesktop
        visible: mode === "desktop"
        anchors.fill: parent
    }

    MainMobile {
        visible: mode === "mobile"
        anchors.fill: parent
        simulator: mySimulator
    }

    Shortcut {
        sequence: StandardKey.AddTab
        context: Qt.ApplicationShortcut
        onActivated: {
            if(mode === "desktop") {
                mode = "mobile"
            } else {
                mode = "desktop"
                //                tempPlot.xMax = mySimulator.simulationTime
                //                tempPlot.xMin = mySimulator.simulationTime-1
                //                maxValue = Math.max(maxValue, value)
                //                tempPlot.yMax = maxValue
            }
        }
    }

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
