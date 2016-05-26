import QtQuick 2.6
import QtQuick.Controls 1.4
import Atomify 1.0

TabView {
    id: tabViewRoot
    property AtomifySimulator simulator
    property EditorTab editorTab
    property alias renderingTab: renderingTab
    property string controlName: {
        if(Qt.platform.os === "osx") {
            return "⌘"
        }
        return "Ctrl"
    }

    Tab {
        id: editorTab
        title: "Script editor "+controlName+"1‌‌"
        anchors.fill: parent
        property TextArea consoleOutput: item ? item.consoleOutput : null
        property LammpsEditor lammpsEditor: item ? item.lammpsEditor : null
        function runScript() {
            if(lammpsEditor) {
                lammpsEditor.runScript()
            }
        }

        function reportError() {
            if(item) {
                item.reportError()
            }
        }

        EditorTab {
            id: myEditorTab
            Component.onCompleted: {
                tabViewRoot.editorTab = myEditorTab
            }

            anchors.fill: parent
            simulator: desktopRoot.simulator
            visualizer: desktopRoot.visualizer
            onVisualizerChanged: {
                console.log("Visualizer: ", visualizer)
            }
        }
    }

    Tab {
        id: renderingTab
        anchors.fill: parent
        title: "Analysis "+controlName+"2"
        AnalysisTab {
            anchors.fill: parent
            atomifyVisualizer: visualizer
            system: simulator ? simulator.system : null
            onItemsChanged: {
                for(var i in items) {
                    var controller = items[i]
                    for(var j in controller.simulatorControls) {
                        var control = controller.simulatorControls[j]
                        control.parent = simulator
                    }
                }
            }
        }
    }
}

