import QtQuick 2.5
import "qrc:/mobile/style"

Text {
    id: welcomeText
    color: Style.font.heading.color
    font.weight: Font.Light
    font.family: "Roboto"
    font.pixelSize: Style.font.heading.size

    renderType: Style.renderType
}
