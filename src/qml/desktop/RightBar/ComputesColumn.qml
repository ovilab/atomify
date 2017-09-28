import QtQuick 2.5
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.2
import QtQuick.Controls 1.4 as QQC1
import Atomify 1.0
import "../../plotting"
import "SimulatorControlItems"

Column {
    id: root
    property var system

    function createComputeWindow(compute, point) {
        var qmlFile = "../../plotting/Plot1D.qml"
        var component = Qt.createComponent(qmlFile);
        if (component.status === Component.Ready) {
            var computePlotter = component.createObject(root);
            computePlotter.x = point.x - computePlotter.width*0.5
            computePlotter.y = point.y - computePlotter.height*0.5
            computePlotter.control = compute
            computePlotter.show()
        }
    }

    Label {
        text: "No computes"
        visible: list.count==0
    }

    Repeater {
        id: list
        model: system ? system.computes.model : null
        delegate: SimulatorControlItem {
            simulatorControl: model.modelData
            visible: list.visible
            height: 20
        }
    }
}

