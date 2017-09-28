import QtQuick 2.5
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.2
import Atomify 1.0
import "../../plotting"
import "SimulatorControlItems"

Column {
    id: root
    property var system
    
    function createPlotWindow(fix, point) {
        var qmlFile = "../../plotting/Plot1D.qml"
        var component = Qt.createComponent(qmlFile);
        if (component.status === Component.Ready) {
            console.log("Can create now. it is ready")
            var plotter = component.createObject(root);
            plotter.x = point.x - plotter.width*0.5
            plotter.y = point.y - plotter.height*0.5
            plotter.control = fix
            plotter.show()
        } else {
            console.log("Error :(")
        }
    }

    Label {
        text: "No fixes"
        visible: list.count==0
    }
    
    Repeater {
        id: list
        model: system ? system.fixes.model : null
        delegate: Loader {
            source: model.modelData.qmlFileName

            onLoaded: {
                item.simulatorControl = model.modelData
                item.height = 20
            }
        }
    }
}
