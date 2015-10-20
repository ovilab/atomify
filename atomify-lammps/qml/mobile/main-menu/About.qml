import QtQuick 2.5
import "qrc:/mobile/style"

Item {
    width: 100
    height: 62

    Heading {
        id: aboutHeading
        anchors {
            top: parent.top
            horizontalCenter: parent.horizontalCenter
            topMargin: Style.baseMargin
        }
        text: "About"
    }

    Flickable {
        id: aboutFlickable
        anchors {
            left: parent.left
            right: parent.right
            top: aboutHeading.bottom
            bottom: parent.bottom
            margins: Style.baseMargin
        }

        Text {
            id: aboutText

            width: aboutFlickable.width

            font.pixelSize: Style.font.size
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            color: Style.font.color
            textFormat: Text.RichText
            text: "<p>"+
                  "Atomify is a molecular dynamics simulator developed by Anders Hafreager and Svenn-Arne Dragly." +
                  "</p>" +
                  "<p>" +
                  "In molecular dynamics, the atoms \"feel\" each other through action-at-a-distance forces. " +
                  "These forces depend on the type of atoms and their environment. " +
                  "</p>" +
                  "<p>Most of the simulations in this application assume the atoms to behave like those in noble gases," +
                  "such as Argon (Ar). " +
                  "The behavior is similar to that found between molecules in other gases, such as O<sub>2</sub>." +
                  "In other words, each atom in the simulation represents an O<sub>2</sub> molecule</p>"
        }
    }
}

