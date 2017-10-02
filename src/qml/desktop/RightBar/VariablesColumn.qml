import QtQuick 2.5
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.2
import Atomify 1.0
import "../../plotting"
import "SimulatorControlItems"

Column {
    id: root
    property var system

    function createPlotWindow(control, point) {
        var qmlFile = "../../plotting/Plot1D.qml"
        var component = Qt.createComponent(qmlFile);
        if (component.status === Component.Ready) {
            var plotter = component.createObject(root);
            plotter.x = point.x - plotter.width*0.5
            plotter.y = point.y - plotter.height*0.5
            plotter.control = control
            plotter.show()
        } else {
            console.log("QML Error, could not load Plot1D for variable ", control.identifier)
        }
    }

    Label {
        text: "No variables"
        visible: list.count==0
    }

    Repeater {
        id: list
        model: system ? system.variables.model : null
        delegate: Loader {
            source: model.modelData.qmlFileName

            onLoaded: {
                item.simulatorControl = model.modelData
                item.height = 20
            }
        }
    }
}

