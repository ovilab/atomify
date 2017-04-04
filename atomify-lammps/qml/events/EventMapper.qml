import QtQuick 2.7
import QtQuick.Controls 2.0
import Qt.labs.settings 1.0
//import QtQuick.Controls 1.5
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.2
import Atomify 1.0

import "."

Connections {
    // TODO: This is deprecated and should be replaced by EventCatcher

    target: EventCenter
    property var mapping
    onTriggered: {
        console.log("Received command", name)
        
        for(var needle in mapping) {
            if(name.substring(0, needle.length) === needle) {
                var command = name.substring(needle.length + 1)
                if(command.indexOf(".") > -1) {
                    continue
                }
                
                var receiver = mapping[needle]
                console.log("Calling", receiver, "with", command)
                receiver[command]()
                break
            }
        }
    }
}
