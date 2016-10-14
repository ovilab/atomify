import QtQuick 2.7
import QtQuick.Controls 2.0

Rectangle {
    id: root
    signal continueClicked()
    signal ljClicked()
    signal examplesClicked()
    signal newTabClicked()

    property bool welcome: true
    property bool finished: false
    property string finishedText:
        "
<style>
h2 { text-align: center; }
a { font-weight: bold; color: #56b1b4; text-decoration: none; }
</style>
<h2>Script finished</h2>
You can continue simulating by pressing the play button (press space), or click <a href=\"continue\">here</a>.
"
    property string welcomeText:
        "
<style>
h2 { text-align: center; }
a { font-weight: bold; color: #56b1b4; text-decoration: none; }
</style>
<h2>Welcome to Atomify</h2>
Atomify is a live LAMMPS editor.
<ul>
<li>Open the <a href=\"LJ\">Lennard Jones</a> example to quickly get started with LAMMPS.</li>
<li>Check out the <a href=\"examples\">other examples</a></li>
<li>Start a <a href=\"newTab\">new script</a> (Ctrl+N)</li>
</ul>
"
    radius: 2
    border.color: "white"
    border.width: 1
    color: Qt.rgba(0.5, 0.5, 0.5, 0.5)

    TextArea {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.fill: parent
        anchors.margins: 10
        color: "white"
        textFormat: TextEdit.RichText
        font.family: "DejaVu Sans Mono"
        font.pixelSize: 16
        readOnly: true
        wrapMode: TextArea.WordWrap
        text: {
            if(welcome) {
                return welcomeText
            } else if(finished) {
                return finishedText
            }
            return ""
        }

        onLinkActivated: {
            if(link==="continue") {
                root.continueClicked()
            } else if(link==="LJ") {
                root.ljClicked()
            } else if(link==="examples") {
                root.examplesClicked()
            } else if(link==="newTab") {
                root.newTabClicked()
            }
        }
    }
}
