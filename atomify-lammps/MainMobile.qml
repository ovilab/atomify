import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Window 2.0
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0
import MySimulator 1.0
import SimVis 1.0
import Compute 1.0
import LammpsOutput 1.0
import AtomStyle 1.0
import Qt.labs.settings 1.0

Item {
    id: mobileRoot

    property MySimulator simulator: null

    AtomifyVisualizer {
        simulator: mobileRoot.simulator
        anchors.fill: parent
    }
}

