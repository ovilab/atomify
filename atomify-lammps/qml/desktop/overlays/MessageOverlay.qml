import QtQuick 2.7
import QtQuick.Controls 2.0


TextArea {
    id: root
    signal continueClicked()
    property string finishedText:
        "
        <style>
        h2 { text-align: center; }
        a { font-weight: bold; color: #56b1b4; text-decoration: none; }
        </style>
        <h2>Script finished</h2>
        You can continue simulating by pressing the play button (press space), or click <a href=\"continue\">here</a>.
        "

    anchors.horizontalCenter: parent.horizontalCenter
    anchors.fill: parent
    anchors.margins: 10
    color: "white"
    textFormat: TextEdit.RichText
    font.family: "DejaVu Sans Mono"
    font.pixelSize: 16
    readOnly: true
    wrapMode: TextArea.WordWrap
    text: finishedText

    onLinkActivated: {
        if(link==="continue") {
            root.continueClicked()
        }
    }
}
