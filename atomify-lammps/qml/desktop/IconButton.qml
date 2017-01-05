import QtQuick 2.7
import QtQuick.Controls.Material 2.0
import QtQuick.Controls 2.0
import QtQuick.Controls 1.5 as QQC1
import QtQuick.Window 2.0
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0
import QtQml.StateMachine 1.0 as DSM
import Atomify 1.0
import SimVis 1.0
import Qt.labs.settings 1.0
import "../visualization"
import "editor"
import "items"
import "RightBar"
import "../plotting"
Image {
    id: root

    signal clicked

    width: 48
    height: width
    fillMode: Image.PreserveAspectFit
    mipmap: true
    
    MouseArea {
        anchors.fill: parent
        onClicked: {
            root.clicked()
        }
    }
}
