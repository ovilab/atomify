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

ApplicationWindow {
    id: applicationRoot

    title: qsTr("Atomify")
    width: 960
    height: 640
    visible: true

    property string mode: {
//        if(["android", "ios", "winphone"].indexOf(Qt.platform.os) > -1) {
            return "mobile"
//        }
//        return "desktop"
    }

    Settings {
        id: settings
        //        property alias mode: applicationRoot.mode
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

//    MainDesktop {
//        visible: mode === "desktop"
//        anchors.fill: parent
//    }

    MainMobile {
        id: mainMobile
        visible: mode === "mobile"
        anchors.fill: parent
    }

    Shortcut {
        property int index: 0
        sequence: "Ctrl+S"
        context: Qt.ApplicationShortcut
        onActivated: {
            mainMobile.grabToImage(function(result) {
                var filename = "/projects/atomify/screenshots/full_"+index+".png"
                console.log("Saving image with filename ", filename)
                result.saveToFile(filename)

                mainMobile.visualizer.grabToImage(function(result) {
                    var filename = "/projects/atomify/screenshots/vis_"+index+".png"
                    console.log("Saving image with filename ", filename)
                    result.saveToFile(filename)
                    index = index + 1
                })
            })
        }
    }

    Shortcut {
        property var widths: [960, 1136, 1334, 2208, 2048, 2732]
        property var heights: [640, 640, 750, 1242, 1536, 2048]
        property int index: 0
        sequence: "Tab"
        context: Qt.ApplicationShortcut
        onActivated: {
            console.log("Changing resolution to ", widths[index], " x ", heights[index])
            applicationRoot.width = widths[index]
            applicationRoot.height = heights[index]

            index = index + 1
            if(index > 5) {
                index = 0;
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
