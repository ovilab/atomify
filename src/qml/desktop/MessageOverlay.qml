import QtQuick 2.7
import QtQuick.Controls 2.1

import ".."
import "../events"

Rectangle {
    id: root
    signal continueClicked()
    signal editClicked()
    signal openClicked()
    signal examplesClicked()
    signal newTabClicked()
    signal hideClicked()

    onHideClicked: {
        hideClickedAtLeastOnce = true
        cantWrite = false
        visible = false
    }

    Timer {
        interval: 1000
        repeat: true
        running: true
        onTriggered: console.log("Welcome now: ", welcome)
    }

    property bool hideClickedAtLeastOnce: false // TODO: state machinery should be used...
    property bool welcome: true
    property bool finished: false
    property bool crashed: false
    property bool cancelling: false
    property bool cantWrite: false // TODO: rename to showCantWrite
    property string cantWriteMacOSAppStoreText:
        "
<style>
h2 { text-align: center; }
a { font-weight: bold; color: #56b1b4; text-decoration: none; }
</style>
<h2>File access problem:</h2>
<p>This simulation is in a directory which Atomify cannot write to. Due to limitations<br>
to file access in the Mac App Store, your simulations must be in the Downloads folder. </p>"

    property string cancellingText:
        "
<style>
h2 { text-align: center; }
a { font-weight: bold; color: #56b1b4; text-decoration: none; }
</style>
<h2>Cancelling ...</h2>
<p>Cancelling simulation. This may take some time.</p>
"
    property string finishedText:
        "
<style>
h2 { text-align: center; }
a { font-weight: bold; color: #56b1b4; text-decoration: none; }
</style>
<h2>Script finished</h2>
<p>You can continue simulating by pressing the <a href=\"continue\">play button</a> (Space).</p>
"
    property string errorMessage: ""
    property string crashedText:
        "
<style>
h2 { text-align: center; }
a { font-weight: bold; color: #56b1b4; text-decoration: none; }
</style>
<h2>Simulation crashed</h2>
" + errorMessage

    property string welcomeText:
        "
<style>
h2 { text-align: center; }
a { font-weight: bold; color: #56b1b4; text-decoration: none; }
</style>
<h2>Welcome to Atomify</h2>
Atomify is a live LAMMPS editor. Here you can edit, run and analyze your simulations.
<div style=\"margin-top: 5px\">&bull; Edit the current script in the <a href=\"edit\">edit</a> menu. (" + EventCenter.nativeText("mode.edit") + ")</div>
<div style=\"margin-top: 5px\">&bull; Check out the <a href=\"examples\">other examples</a> (" + EventCenter.nativeText("mode.examples") + ")</div>
<div style=\"margin-top: 5px\">&bull; Start a <a href=\"newTab\">new script</a> (" + EventCenter.nativeText("editor.new") + ")</div>
<div style=\"margin-top: 5px\">&bull; Open one of your <a href=\"open\">existing scripts</a> (" + EventCenter.nativeText("editor.open") + ")</div>
<br>
Learn more about Atomify at <a href=\"website\">ovilab.net/atomify</a>. We love feedback, bugreports and ideas for new features.
"

    color: Qt.rgba(0.1, 0.1, 0.1, 0.7)

    Label {
        id: text
        anchors {
            centerIn: parent
            verticalCenterOffset: -parent.height * 0.2
        }
        textFormat: TextEdit.RichText
        wrapMode: TextArea.WordWrap
        text: {
            if(cantWrite) {
                return cantWriteMacOSAppStoreText
            } else if(welcome) {
                return welcomeText
            } else if(finished) {
                return finishedText
            } else if(crashed) {
                return crashedText
            } else if(cancelling) {
                return cancellingText
            }
            return ""
        }

        onLinkActivated: {
            if(link==="continue") {
                root.continueClicked()
            } else if(link==="edit") {
                root.editClicked()
            } else if(link==="examples") {
                root.examplesClicked()
            } else if(link==="newTab") {
                root.newTabClicked()
            } else if(link==="open") {
                root.openClicked()
            } else if(link==="hide") {
                root.hideClicked()
            } if(link==="website") {
                Qt.openUrlExternally("http://ovilab.net/atomify")
            }
        }

        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.NoButton
            propagateComposedEvents: true
            cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : undefined
        }
    }

    Button {
        anchors {
            horizontalCenter: text.horizontalCenter
            top: text.bottom
            margins: 24
        }

        text: "Ok, got it"
        onClicked: root.hideClicked()
    }
}
