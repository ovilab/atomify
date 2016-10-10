import QtQuick 2.7
import QtQuick.Controls 2.0

TextArea {
    id: root
    signal ljClicked()
    signal examplesClicked()
    signal newScriptClicked()
    color: "white"
    textFormat: TextEdit.RichText
    font.family: "DejaVu Sans Mono"
    font.pixelSize: 12
    readOnly: true
    text:
"<h2>Welcome to Atomify</h2>
<ul>
    <li>Open the <a href=\"LJ\">Lennard Jones</a> example to quickly get started with LAMMPS.</li>
    <li>Check out the <a href=\"examples\">other examples</a></li>
    <li>Start a new script (Ctrl+N)</li>
</ul>
"

    onLinkActivated: {
        if(link==="LJ") {
            root.ljClicked()
        } else if(link==="examples") {
            root.examplesClicked()
        } else if(link==="newTab") {
            root.newScriptClicked()
        }
    }
}
