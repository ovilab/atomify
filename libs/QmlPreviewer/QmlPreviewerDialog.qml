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

    function refresh() {
        folderListModel.folder = "file:///tmp"
        requestStart()
        folderListModel.folder = "qrc:/qtqmlpreview"
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
                    anchors {
                        horizontalCenter: parent.horizontalCenter
                    }
                    text: "Select project path"
                }

                Button {
                    anchors {
                        horizontalCenter: parent.horizontalCenter
                    }
                    text: "Browse"
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

                    text: "Add .qrc"
                    onClicked: qrcDialog.open()
                }

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
                        folder: "qrc:/qtqmlpreview"
                        rootFolder: "qrc:/qtqmlpreview"
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
                                root.filePath = fileURL.toString().replace("file::/", "qrc:/")
                            }
                        }
                    }
                }

                //            ListView {
                //                id: fileView
                //                anchors {
                //                    left: parent.left
                //                    right: parent.right
                //                }
                //                height: 300

                //                model: XmlListModel {
                //                    id: xmlModel
                //                    source: "file:///home/svenni/Sync/projects/atomify/atomify-lammps/atomify-lammps/qml/qml.qrc"
                //                    query: "/RCC/qresource/file"

                //                    XmlRole { name: "filename"; query: "string()" }
                //                }
                //                delegate: ItemDelegate {
                //                    text: filename
                //                    onClicked: fileView.currentIndex = index
                //                }
                //                highlight: Rectangle {
                //                    color: "grey"
                //                }
                //            }

                //            ListView {
                //                anchors {
                //                    left: parent.left
                //                    right: parent.right
                //                }

                //                height: 300

                //                model: XmlListModel {
                //                    id: xmlModel
                //                    source: "file:///tmp/test.xml"
                //                    query: "/catalog/book"
                //                    // query the book title
                //                    XmlRole { name: "title"; query: "title/string()" }

                //                    // query the book's year
                //                    XmlRole { name: "year"; query: "year/number()" }

                //                    // query the book's type (the '@' indicates 'type' is an attribute, not an element)
                //                    XmlRole { name: "type"; query: "@type/string()" }

                //                    // query the book's first listed author (note in XPath the first index is 1, not 0)
                //                    XmlRole { name: "first_author"; query: "author[1]/string()" }

                //                    // query the wanted attribute as a boolean
                //                    XmlRole { name: "wanted"; query: "boolean(@wanted)" }
                //                }
                //                delegate: Column {
                //                    Text { text: title + " (" + type + ")"; font.bold: wanted }
                //                    Text { text: first_author }
                //                    Text { text: year }
                //                }
                //            }

                //            Label {
                //                anchors {
                //                    left: parent.left
                //                    right: parent.right
                //                }
                //                wrapMode: Label.WrapAtWordBoundaryOrAnywhere

                //                text: cleanPath(filePath)
                //            }

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
