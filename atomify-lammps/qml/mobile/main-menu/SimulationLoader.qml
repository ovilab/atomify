import QtQuick 2.0

Loader {
    property string folder

    function identifier(folder) {
        var names = folder.split("/")
        if(folder === "") {
            return ""
        }

        return names[names.length - 1]
    }

    source: folder + "/" + identifier(folder) + ".qml"
    onLoaded: {
        item.folder = folder
    }
}

