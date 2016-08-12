import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Controls 1.4 as QQC1
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
import "plotting"
QQC1.ApplicationWindow {
    id: applicationRoot
    visible: true
    title: "Atomify"

    QQC1.MenuBar {
        QQC1.Menu {
            title: "File"
            QQC1.MenuItem {
                text: "New script"
            }
            QQC1.MenuItem {
                text: "Open"
            }
            QQC1.MenuItem {
                text: "Save"
            }
            QQC1.MenuItem {
                text: "Save as"
            }
        }
    }

    Settings {
        id: settings
        property alias width: applicationRoot.width
        property alias height: applicationRoot.height
    }

    function resetStyle() {
        Style.reset(width, height, Screen)
    }

    onWidthChanged: {
        console.log("Width changed")
        resetStyle()
    }

    onHeightChanged: {
        console.log("Height changed")
        resetStyle()
    }

    Component.onCompleted: {
        resetStyle()
    }

    MainDesktop {
        id: mainDesktop
        anchors.fill: parent
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

//    Plot2D {
//        id: plot2d
//    }
//    Button {
//        text: "Open"
//        onClicked: {
//            plot2d.show()
//        }
//    }
}
