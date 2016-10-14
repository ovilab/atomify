import QtQuick 2.7
import QtQuick.Controls 2.0

TextArea {
    id: root
    signal ljClicked()
    signal examplesClicked()
    signal newTabClicked()
    color: "white"
    textFormat: TextEdit.RichText
    font.family: "DejaVu Sans Mono"
    font.pixelSize: 16
    wrapMode: TextArea.WordWrap
    readOnly: true
    text:
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

    onLinkActivated: {
        if(link==="LJ") {
            root.ljClicked()
        } else if(link==="examples") {
            root.examplesClicked()
        } else if(link==="newTab") {
            root.newTabClicked()
        }
    }
}
