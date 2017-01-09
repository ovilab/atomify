import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Controls.Material 2.0
import SimVis 1.0

Pane {
    padding: 32
    Flickable {
        anchors.fill: parent
        contentHeight: content.height
        ScrollBar.vertical: ScrollBar {}
        Column {
            id: content
            anchors {
                left: parent.left
                right: parent.right
            }

            spacing: 24

            Label {
                text: "Help"
                font.pixelSize: 32
            }

            Label {
                textFormat: Text.RichText
                text: "
<p>Atomify is a live LAMMPS editor. Here you can edit, run and analyze your scripts.</p>
<p>You can find the LAMMPS documentation online.</p>
<p>Special Atomify commands can be run by starting a comment with a slash: '#/command [arguments ...]'</p>
<p>Here are some of the built-in Atomify commands:</p>
<p>&bull; Generate bonds for certain distances:</p>
<p><pre>    #/bond 1 2 1.5</pre></p>
<p>&bull; Set atom colors:</p>
<p><pre>    #/color 1 blue</pre></p>
<p>&bull; Set atom type:</p>
<p><pre>    #/atom 1 hydrogen</pre></p>
<p>&bull; Set camera position and view center:</p>
<p><pre>    #/camera position 10 20 30</pre></p>
<p><pre>    #/camera viewcenter 5 5 5</pre></p>
"
            }
        }
    }
}
