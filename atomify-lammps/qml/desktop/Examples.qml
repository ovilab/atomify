import QtQuick 2.7
import QtQuick.Controls 2.0
import Atomify 1.0

Pane {
    id: root

    signal simulationClicked(var script)
    property Item screenshotObject
    property string currentScript

    property string serverUrl: "http://146.185.171.238:1338/parse"

    Component.onCompleted: {
        refresh()
    }

    function upload() {
        var name = nameField.text
        var description = descriptionField.text
        if(!screenshotObject) {
            console.warn("screenshotObject must be set to allow uploads")
            return
        }

        screenshotObject.grabToImage(function(result) {
            console.log("Grabbed screenshot")
            parseFileUploader.uploadGrabbedImage(result, function(uploadResult) {
                console.log("Got data", uploadResult)
                var uploadData = JSON.parse(uploadResult);

                var status
                var req = new XMLHttpRequest;
                req.open("POST", serverUrl + "/classes/Simulation");
                req.setRequestHeader("X-Parse-Application-Id", "atomify");
                var data = {
                    name: name,
                    description: description,
                    script: currentScript,
                    image: {
                        "name": uploadData.name,
                        __type: "File"
                    }
                }
                req.onreadystatechange = function() {
                    status = req.readyState;
                    if (status === XMLHttpRequest.DONE) {
                        console.log(req.responseText);
                        refresh()
                    }
                }
                console.log("Sending", JSON.stringify(data))
                req.send(JSON.stringify(data))
            })
//            var status
//            var req = new XMLHttpRequest;
//            req.open("POST", serverUrl + "/files/screenshot.png");
//            req.setRequestHeader("X-Parse-Application-Id", "atomify");
////            req.setRequestHeader("Content-Type", "image/png");
//            req.onreadystatechange = function() {
//                status = req.readyState;
//                if (status === XMLHttpRequest.DONE) {
//                    console.log(req.responseText);
//                    refresh()
//                    var response = JSON.parse(req.responseText)
//                    Qt.openUrlExternally(response.url)
//                }
//            }
//            req.send(JSON.stringify({data:"image/png", base64: base64}));
        })
//        req.send(JSON.stringify(data));
    }

    function refresh() {
        var status
        var wasLoading
        var req = new XMLHttpRequest;
        req.open("GET", serverUrl + "/classes/Simulation");
        req.setRequestHeader("X-Parse-Application-Id", "atomify");
        req.onreadystatechange = function() {
            status = req.readyState;
            if (status === XMLHttpRequest.DONE) {
                var objectArray = JSON.parse(req.responseText);
                if (objectArray.errors !== undefined)
                    console.log("Error fetching tweets: " + objectArray.errors[0].message)
                else {
                    listModel.clear()
                    for(var i in objectArray.results) {
                        var simulation = objectArray.results[i]
                        listModel.append(objectArray.results[i])
                    }
                }
                if (wasLoading == true) {
                    console.log("Is loaded")
                }
            }
            wasLoading = (status === XMLHttpRequest.LOADING);
        }
        req.send();
    }

    ParseFileUploader {
        id: parseFileUploader
    }

    ListView {

        anchors {
            top: parent.top
            right: parent.right
            left: parent.left
            bottom: uploadColumn.top
        }

        clip: true
        model: ListModel { id: listModel }
        delegate: Item {
            width: parent.width
            height: parent.width / 2

            Image {
                id: exampleImage
                anchors {
                    right: parent.right
                    top: parent.top
                    bottom: parent.bottom
                    margins: 8
                }
                width: height
                fillMode: Image.PreserveAspectCrop
                source: image ? image.url : ""
                smooth: true
                antialiasing: true
                mipmap: true
            }

            Label {
                id: exampleName
                anchors {
                    margins: 8
                    left: parent.left
                    top: parent.top
                    right: exampleImage.left
                }
                font.weight: Font.Bold
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere

                text: name
            }

            Label {
                anchors {
                    margins: 8
                    left: parent.left
                    top: exampleName.bottom
                    right: exampleImage.left
                }
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere

                text: description
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    simulationClicked(script)
                }
            }
        }
    }

    Column {
        id: uploadColumn
        anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }

        Label {
            text: "Upload your current simulation"
        }

        Label {
            text: "Name"
        }

        TextField {
            id: nameField
            text: "Name"
        }

        Label {
            text: "Description"
        }

        TextField {
            id: descriptionField
            text: "Description"
        }

        Button {
            id: uploadButton
            text: "Upload"

            onClicked: {
                upload()
            }
        }
    }
}
