import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Window 2.0
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0
import Atomify 1.0
import SimVis 1.0
import Qt.labs.settings 1.0

import "../visualization"

Rectangle {
    id: tabDisable
    property bool isVisible: false
    property alias showTabDisable: show
    property alias hideTabDisable: hide
    width: tabDisableLabel.width + 16
    height: tabDisableLabel.height + 10
    opacity: 0
    radius: 5
    
    NumberAnimation on opacity {
        id: hide
        to: 0; duration: 200
    }
    
    NumberAnimation on opacity {
        id: show
        to: 0.8; duration: 200
    }
    
    Label {
        id: tabDisableLabel
        x: 8
        y: 5
        text: "Press Tab to disable focus mode (click to hide)"
    }
    MouseArea {
        anchors.fill: parent
        onClicked: {
            hide.start()
        }
    }
}
