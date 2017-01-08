import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Controls 1.4 as QQC1
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0
import Atomify 1.0
import SimVis 1.0
import Qt.labs.settings 1.0
import QtCharts  2.0
import KeySequence 1.0
//import Qt.labs.platform 1.0
import "mobile"
import "mobile/style"
import "desktop"
import "plotting"
import "."

QQC1.ApplicationWindow {
    id: applicationRoot
    title: "Atomify LAMMPS"
    visible: true

    width: 1280
    height: 1024

    menuBar: QQC1.MenuBar {
        QQC1.Menu {
            title: "File"
            QQC1.MenuItem {
                text: "New script"
                shortcut: StandardKey.New
                onTriggered: {
                    EventCenter.postEvent("editorWindow.newTab")
                }
            }
            QQC1.MenuItem {
                text: "Open"
                shortcut: StandardKey.Open
                onTriggered: {
                    EventCenter.postEvent("editorWindow.openTab")
                }
            }
            QQC1.MenuItem {
                text: "Close"
                shortcut: StandardKey.Close
                onTriggered: {
                    EventCenter.postEvent("editorWindow.closeTab")
                }
            }
            QQC1.MenuItem {
                text: "Save"
                shortcut: StandardKey.Save
                onTriggered: {
                    EventCenter.postEvent("editor.save")
                }
            }
            QQC1.MenuItem {
                text: "Save as"
                shortcut: StandardKey.SaveAs
                onTriggered: {
                    EventCenter.postEvent("editor.saveAs")
                }
            }
        }
        QQC1.Menu {
            title: "Edit"
            QQC1.MenuItem {
                text: "Undo"
                enabled: mainDesktop.editor.editorWindow.currentEditor && mainDesktop.editor.editorWindow.currentEditor.textArea.canUndo
                shortcut: StandardKey.Undo
                onTriggered: {
                    EventCenter.postEvent("editor.textArea.undo")
                }
            }
            QQC1.MenuItem {
                text: "Redo"
                enabled: mainDesktop.editor.editorWindow.currentEditor && mainDesktop.editor.editorWindow.currentEditor.textArea.canRedo
                shortcut: StandardKey.Redo
                onTriggered: {
                    EventCenter.postEvent("editor.textArea.redo")
                }
            }
            QQC1.MenuSeparator { }

            QQC1.MenuItem {
                text: "Cut"
                enabled: mainDesktop.editor.editorWindow.currentEditor && mainDesktop.editor.editorWindow.currentEditor.textArea.selectedText.length > 0
                shortcut: StandardKey.Cut
                onTriggered: {
                    EventCenter.postEvent("editor.textArea.cut")
                }
            }
            QQC1.MenuItem {
                text: "Copy"
                enabled: mainDesktop.editor.editorWindow.currentEditor && mainDesktop.editor.editorWindow.currentEditor.textArea.selectedText.length > 0
                shortcut: StandardKey.Copy
                onTriggered: {
                    EventCenter.postEvent("editor.textArea.copy")
                }
            }
            QQC1.MenuItem {
                text: "Paste"
                enabled: mainDesktop.editor.editorWindow.currentEditor && mainDesktop.editor.editorWindow.currentEditor.textArea.canPaste
                shortcut: StandardKey.Paste
                onTriggered: {
                    EventCenter.postEvent("editor.textArea.paste")
                }
            }
        }
        QQC1.Menu {
            title: "Camera"
            QQC1.MenuItem {
                text: "Switch camera"
                shortcut: "C"
                onTriggered: {
                    EventCenter.postEvent("desktop.changeMode")
                }
            }
            QQC1.MenuItem {
                text: "Reset to origin"
                shortcut: "O"
                onTriggered: {
                    EventCenter.postEvent("visualizer.resetToSystemCenter")
                }
            }
        }
        QQC1.Menu {
            title: "Run"
            QQC1.MenuItem {
                text: "Run script"
                shortcut: "Ctrl+R"
                onTriggered: {
                    EventCenter.postEvent("editorWindow.runScript")
                }
            }
            QQC1.MenuItem {
                text: mainDesktop.simulator.states.paused.active ? "Unpause" : "Pause"
                enabled: mainDesktop.simulator.states.parsing.active || mainDesktop.simulator.states.continued.active || mainDesktop.simulator.states.paused.active
                shortcut: StandardKey.Print
                onTriggered: {
                    mainDesktop.simulator.togglePaused()
                }
            }
            QQC1.MenuItem {
                text: "Increase simulation speed"
                enabled: mainDesktop.simulator.simulationSpeed < 100
                shortcut: "+"
                onTriggered: {
                    EventCenter.postEvent("simulator.increaseSimulationSpeed")
                }
            }
            QQC1.MenuItem {
                text: "Decrease simulation speed"
                enabled: mainDesktop.simulator.simulationSpeed > 1
                shortcut: "-"
                onTriggered: {
                    EventCenter.postEvent("simulator.decreaseSimulationSpeed")
                }
            }
        }

        QQC1.Menu {
            title: "Rendering"
            QQC1.MenuItem {
                text: "Capture screenshot"
                // shortcut: "Ctrl+R"
                onTriggered: {
                    mainDesktop.captureScreenshot()
                }
            }
        }
    }

    Settings {
        id: settings
        property alias width: applicationRoot.width
        property alias height: applicationRoot.height
        property alias renderQuality: mainDesktop.renderQuality
    }

    function resetStyle() {
        Style.reset(width, height, Screen)
    }

    onWidthChanged: {
        console.log("Width changed")
        resetStyle()
    }

    onHeightChanged: {
        console.log("Height changed")
        resetStyle()
    }

    Component.onCompleted: {
        resetStyle()
        mainDesktop.visualizer.mainCompleted = true
    }

    MainDesktop {
        id: mainDesktop
        anchors.fill: parent
        mouseMover: MouseMover {
            id: mouseMover
            window: applicationRoot
        }
    }

    Shortcut {
        sequence: StandardKey.FullScreen
        context: Qt.ApplicationShortcut
        onActivated: {
            if(visibility === Window.FullScreen) {
                visibility = Window.AutomaticVisibility
            } else {
                visibility = Window.FullScreen
            }
        }
    }
}
