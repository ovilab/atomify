pragma Singleton
import QtQuick 2.7
import KeySequence 1.0

QtObject {
    signal triggered(string name, var data)

    property var definitions: []

    function postEvent(name, data) {
        // console.log("Posting event", name, data)
        triggered(name, data)
    }

    function register(Shortcut) {
        var defs = definitions.slice()
        defs.push(Shortcut)
        definitions = defs
    }

    function unregister(Shortcut) {
        var index = definitions.indexOf(Shortcut)
        if(index < 0) {
            console.log("Warning: EventCenter.unregister: Object not registered, so not removed.")
        }
        var defs = definitions.slice()
        defs.splice(index, 1)
        definitions = defs
    }

    function nativeText(name) {
        for(var i in definitions) {
            var definition = definitions[i]
            if(definition.name === name) {
                var sequence = definition.sequence || definition.shortcut // Shortcut or MenuItem
                return KeySequence.nativeText(sequence)
            }
        }
        return "[" + name + "]"
    }
}
