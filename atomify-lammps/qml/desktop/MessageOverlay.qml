import QtQuick 2.7
import QtQuick.Controls 2.1

Rectangle {
    id: root
    signal continueClicked()
    signal editClicked()
    signal examplesClicked()
    signal newTabClicked()
    signal hideClicked()

    property bool welcome: true
    property bool finished: false
    property bool crashed: false
    property bool cancelling: false
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
Atomify is a live LAMMPS editor. Here you can edit, run and analyze your scripts.
<div style=\"margin-top: 5px\">&bull; Edit the current script in the <a href=\"edit\">edit</a> menu. (Ctrl+2)</div>
<div style=\"margin-top: 5px\">&bull; Check out the <a href=\"examples\">other examples</a> (Ctrl+3)</div>
<div style=\"margin-top: 5px\">&bull; Start a <a href=\"newTab\">new script</a> (Ctrl+N)</div>
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
            if(welcome) {
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
            } else if(link==="hide") {
                root.hideClicked()
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
