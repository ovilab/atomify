import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2
import Qt.labs.settings 1.0

Rectangle {
    id: root

    signal start(var qrcPaths, url filePath)

    property url projectPath
    property var qrcPaths: []
    property string qrcPathsStringified
    property url filePath
    property url source

    function baseName(file) {
        var split = file.toString().split("/")
        return split[split.length - 1]
    }

    function cleanPath(path) {
        return path.toString().replace(/^file:\/\//, "")
    }

    width: 1600
    height: 900
    color: colorDialog.color

    Component.onCompleted: {
        requestStart()
    }

    onQrcPathsStringifiedChanged: {
        var paths = JSON.parse(qrcPathsStringified)
        var newPaths = []
        for(var i in paths) {
            newPaths.push(Qt.resolvedUrl(paths[i]))
        }
        qrcPaths = newPaths
        console.log("Parsed as", qrcPaths)
    }

    onQrcPathsChanged: {
        var stringPaths = []
        for(var i in qrcPaths) {
            stringPaths.push(qrcPaths[i].toString())
        }
        qrcPathsStringified = JSON.stringify(stringPaths)
        console.log("Stringified as", qrcPathsStringified)
    }

    onFilePathChanged: {
        requestStart()
    }

    onProjectPathChanged: {
        requestStart()
    }

    function reload() {
        loader.source = ""
        var clippedName = filePath.toString().replace(projectPath.toString(), "")
        loader.source = "qrc:///qtqmlpreview/" + clippedName
    }

    function requestStart() {
        if(filePath.toString().length > 0 && projectPath.toString().length > 0) {
            start(qrcPaths, filePath)
        }
    }

    Settings {
        property alias projectPath: root.projectPath
        property alias filePath: root.filePath
        property alias qrcPaths: root.qrcPathsStringified
        property alias backgroundColor: colorDialog.color

        category: "qmlPreviewer"
    }

    Item {
        id: pane
        anchors {
            left: parent.left
            top: parent.top
            bottom: parent.bottom
        }

        width: 300

        Column {
            id: column
            spacing: 16

            anchors.fill: parent

            Button {
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }

                text: "Select project path"
                onClicked: folderDialog.open()
            }

            Label {
                anchors {
                    left: parent.left
                    right: parent.right
                }
                wrapMode: Label.WrapAtWordBoundaryOrAnywhere

                text: cleanPath(projectPath)
            }

            Button {
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                text: "Select file to preview"
                onClicked: fileDialog.open()
            }

            Label {
                anchors {
                    left: parent.left
                    right: parent.right
                }
                wrapMode: Label.WrapAtWordBoundaryOrAnywhere

                text: cleanPath(filePath)
            }

            Button {
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }

                text: "Add QRC files"
                onClicked: qrcDialog.open()
            }

            ListView {
                anchors {
                    left: parent.left
                    right: parent.right
                }

                height: 300

                model: root.qrcPaths
                delegate: Rectangle {
                    width: parent.width
                    height: 64
                    Text {
                        anchors.centerIn: parent
                        text: baseName(modelData)
                    }
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            var paths = root.qrcPaths
                            paths.splice(index, 1)
                            root.qrcPaths = paths
                        }
                    }

                }
            }

            Button {
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                text: "Select background color"

                onClicked: colorDialog.open()
            }

            Button {
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                text: "Refresh"

                onClicked: requestStart()
            }
        }
    }

    Item {
        anchors {
            top: parent.top
            bottom: parent.bottom
            left: pane.right
            right: parent.right
        }
        Loader {
            id: loader
            anchors.centerIn: parent
        }
    }

    ColorDialog {
        id: colorDialog
        color: "white"
    }

    FileDialog {
        id: folderDialog

        selectFolder: true

        onAccepted: {
            projectPath = folder
        }
    }

    FileDialog {
        id: qrcDialog

        nameFilters: "*.qrc"

        onAccepted: {
            var paths = root.qrcPaths
            paths.push(fileUrl)
            console.log("Set new paths", paths)
            root.qrcPaths = paths
        }
    }

    FileDialog {
        id: fileDialog

        onAccepted: {
            filePath = fileUrl
        }
    }
}
