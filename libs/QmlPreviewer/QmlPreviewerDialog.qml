import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2
import Qt.labs.settings 1.0
import QtQuick.XmlListModel 2.0
import Qt.labs.folderlistmodel 2.1

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

    Component.onCompleted: {
        refresh()
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
        refresh()
    }

    onProjectPathChanged: {
        refresh()
    }

    function refresh() {
        requestStart()
        folderListModel.folder = ""
        var rootPath = "qrc:/qtqmlpreview"
        console.log("WOOP", folderListModel.folder == "", folderListModel.folder, folderListModel.rootFolder)
//        if(folderListModel.folder.toString().substring(0, rootPath.length) != rootPath) {
            folderListModel.folder = rootPath
//            folderListModel.rootFolder = rootPath
//        }
    }

    function reload() {
        loader.source = ""
//        var clippedName = filePath.toString().replace(projectPath.toString(), "")
//        loader.source = "qrc:///qtqmlpreview/" + clippedName
        loader.source = filePath.toString().replace("file:", "qrc")
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
        property alias canvasX: canvas.x
        property alias canvasY: canvas.y
        property alias canvasWidth: canvas.width
        property alias canvasHeight: canvas.height

        category: "qmlPreviewer"
    }

    Pane {
        id: pane
        anchors {
            left: parent.left
            top: parent.top
            bottom: parent.bottom
        }

        width: 300

        Flickable {
            anchors.fill: parent
            contentHeight: column.height

            Column {
                id: column
                spacing: 16

                anchors {
                    left: parent.left
                    right: parent.right
                }

                Label {
                    text: "QRC files"
                }

                ListView {
                    anchors {
                        left: parent.left
                        right: parent.right
                    }

                    height: 300

                    model: root.qrcPaths
                    delegate: ItemDelegate {
                        text: baseName(modelData)
                        onClicked: {
                            var paths = root.qrcPaths
                            paths.splice(index, 1)
                            root.qrcPaths = paths
                        }
                    }
                }

                Button {
                    anchors {
                        horizontalCenter: parent.horizontalCenter
                    }

                    text: "Add .qrc"
                    onClicked: qrcDialog.open()
                }

                Column {
                    anchors {
                        left: parent.left
                        right: parent.right
                    }

//                    visible: folderListModel.folder != ""

                    Label {
                        anchors {
                            horizontalCenter: parent.horizontalCenter
                        }
                        text: "Select file to preview"
                    }

                    Button {
                        text: "Up"
    //                    enabled: folderListModel.folder != "file::/qtqmlpreview/"
                        onClicked: {
                            folderListModel.folder = folderListModel.parentFolder
                        }
                    }

                    ListView {
                        id: fileView
                        anchors {
                            left: parent.left
                            right: parent.right
                        }
                        height: 300
                        clip: true

                        model: FolderListModel {
                            id: folderListModel
                            showDirsFirst: true
                            showHidden: true
                            folder: ""
                            rootFolder: "qrc:/"
                            onFolderChanged: {
                                console.log("Current folder:", folder)
                            }
                        }
                        delegate: ItemDelegate {
                            text: fileName
                            highlighted: ListView.isCurrentItem
                            onClicked: {
                                if(fileIsDir) {
                                    folderListModel.folder = fileURL
                                } else {
                                    fileView.currentIndex = index
                                    console.log("File URL", fileURL, filePath)
                                    root.filePath = filePath.replace(":/", "qrc:/")
                                }
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

                    onClicked: {
                        refresh()
                    }
                }
            }
        }
    }

    Pane {
        anchors {
            top: parent.top
            bottom: parent.bottom
            left: pane.right
            right: parent.right
        }
        Rectangle {
            id: canvas
            width: 640
            height: 480
            color: colorDialog.color

            MouseArea {
                anchors.fill: parent
                drag.target: parent
            }

            ResizeRectangle {
                target: parent
            }

            Loader {
                id: loader
                anchors {
                    fill: parent
                    margins: 24
                }
                clip: true
            }
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
