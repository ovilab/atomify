pragma Singleton
import QtQuick 2.0

QtObject {
    signal triggered(string name, var data)

    function postEvent(name, data) {
        triggered(name, data)
    }
}
