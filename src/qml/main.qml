import QtQuick 2.7
import QtQuick.Controls 2.2
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
import "events"

QQC1.ApplicationWindow {
    id: applicationRoot
    property string initialScriptFilePath: ""

    title: mainDesktop.title
    visible: true

    width: 1280
    height: 1024

    minimumHeight: 480
    minimumWidth: 640

    menuBar: QQC1.MenuBar {
        QQC1.Menu {
            title: "File"
            EventMenuItem {
                name: "editor.new"
                text: "New script"
                shortcut: StandardKey.New
                onTriggered: {
                    EventCenter.postEvent("mode.edit")
                    EventCenter.postEvent("editor.new")
                }
            }

            EventMenuItem {
                text: "Open"
                name: "editor.open"
                shortcut: StandardKey.Open
                onTriggered: {
                    EventCenter.postEvent("mode.edit")
                    EventCenter.postEvent("editor.open")
                }
            }
            EventMenuItem {
                text: "Close"
                shortcut: StandardKey.Close
                name: "editor.close"
                enabled: applicationRoot.activeFocusItem !== null
                onTriggered: {
                    EventCenter.postEvent("mode.edit")
                    EventCenter.postEvent("editor.close")
                }
            }
            EventMenuItem {
                text: "Save"
                name: "editor.save"
                shortcut: StandardKey.Save
                onTriggered: {
                    EventCenter.postEvent("editor.save")
                }
            }
            EventMenuItem {
                text: "Save as"
                name: "editor.saveAs"
                shortcut: StandardKey.SaveAs
                onTriggered: {
                    EventCenter.postEvent("mode.edit")
                    EventCenter.postEvent("editor.saveAs")
                }
            }
            QQC1.MenuSeparator {

            }

            EventMenuItem {
                id: anonymousUsageStatistics
                text: "Send anonymous usage statistics"
                checkable: true
                checked: mainDesktop.simulator.usageStatistics.sendUsageStatistics
                onTriggered: mainDesktop.simulator.usageStatistics.setSendUsageStatistics(checked)
            }
        }
        QQC1.Menu {
            title: "Edit"
            EventMenuItem {
                text: "Undo"
                name: "editor.undo"
                enabled: mainDesktop.editor.editorWindow.currentEditor && mainDesktop.editor.editorWindow.currentEditor.textArea.canUndo
                shortcut: StandardKey.Undo
                onTriggered: {
                    EventCenter.postEvent("editor.textArea.undo")
                }
            }
            EventMenuItem {
                text: "Redo"
                name: "editor.redo"
                enabled: mainDesktop.editor.editorWindow.currentEditor && mainDesktop.editor.editorWindow.currentEditor.textArea.canRedo
                shortcut: StandardKey.Redo
                onTriggered: {
                    EventCenter.postEvent("editor.textArea.redo")
                }
            }
            QQC1.MenuSeparator { }

            EventMenuItem {
                text: "Cut"
                name: "editor.cut"
                enabled: mainDesktop.editor.editorWindow.currentEditor && mainDesktop.editor.editorWindow.currentEditor.textArea.selectedText.length > 0
                shortcut: StandardKey.Cut
                onTriggered: {
                    EventCenter.postEvent("editor.textArea.cut")
                }
            }
            EventMenuItem {
                text: "Copy"
                name: "editor.copy"
                enabled: mainDesktop.editor.editorWindow.currentEditor && mainDesktop.editor.editorWindow.currentEditor.textArea.selectedText.length > 0
                shortcut: StandardKey.Copy
                onTriggered: {
                    EventCenter.postEvent("editor.textArea.copy")
                }
            }
            EventMenuItem {
                text: "Paste"
                name: "editor.paste"
                enabled: mainDesktop.editor.editorWindow.currentEditor && mainDesktop.editor.editorWindow.currentEditor.textArea.canPaste
                shortcut: StandardKey.Paste
                onTriggered: {
                    EventCenter.postEvent("editor.textArea.paste")
                }
            }
        }
        QQC1.Menu {
            title: "Camera"
            EventMenuItem {
                text: "Switch camera"
                name: "desktop.changeMode"
                shortcut: "C"
                onTriggered: {
                    EventCenter.postEvent("desktop.changeMode")
                }
            }
            EventMenuItem {
                text: "Reset to origin"
                shortcut: "O"
                name: "visualizer.resetToSystemCenter"
                onTriggered: {
                    EventCenter.postEvent("visualizer.resetToSystemCenter")
                }
            }
        }
        QQC1.Menu {
            title: "Run"
            EventMenuItem {
                text: "Run script"
                shortcut: "Ctrl+R"
                name: "editorWindow.runScript"
                onTriggered: {
                    EventCenter.postEvent("editorWindow.runScript")
                }
            }
            EventMenuItem {
                text: mainDesktop.simulator.states.paused.active ? "Unpause" : "Pause"
                enabled: mainDesktop.simulator.states.parsing.active || mainDesktop.simulator.states.continued.active || mainDesktop.simulator.states.paused.active
                shortcut: StandardKey.Print
                onTriggered: {
                    mainDesktop.simulator.togglePause()
                }
            }
            EventMenuItem {
                text: "Increase simulation speed"
                enabled: mainDesktop.simulator.simulationSpeed < 100
                shortcut: "+"
                onTriggered: {
                    EventCenter.postEvent("simulator.increaseSimulationSpeed")
                }
            }
            EventMenuItem {
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
            EventMenuItem {
                text: "Capture screenshot"
                onTriggered: {
                    mainDesktop.captureScreenshot()
                }
            }

            EventMenuItem {
                text: "Full screen"
                name: "app.fullscreen"
                shortcut: StandardKey.FullScreen
                checkable: true
                checked: visibility === Window.FullScreen
                onTriggered: {
                    if(visibility === Window.FullScreen) {
                        visibility = Window.AutomaticVisibility
                    } else {
                        visibility = Window.FullScreen
                    }
                }
            }

            EventMenuItem {
                text: "Show guides"
                name: "visualizer.showguides"
                shortcut: "G"
                checkable: true
                checked: mainDesktop.visualizer.guidesVisible
                onTriggered: {
                    mainDesktop.visualizer.guidesVisible = !mainDesktop.visualizer.guidesVisible
                }
            }

            EventMenuItem {
                text: "Show outline"
                name: "visualizer.showoutline"
                shortcut: "M"
                checkable: true
                checked: mainDesktop.visualizer.systemBoxVisible
                onTriggered: {
                    mainDesktop.visualizer.systemBoxVisible = !mainDesktop.visualizer.systemBoxVisible
                }
            }
        }
    }

    Item {
        id: shortcuts
        EventShortcut {
            sequence: "Ctrl+P"
            name: "simulator.togglePause"
        }

        EventShortcut {
            sequence: "Space"
            name: "simulator.togglePause"
        }

        EventShortcut {
            sequence: "Ctrl+I"
            name: "editor.focus"
        }

        EventShortcut {
            sequence: "Escape"
            name: "escape"
        }

        EventShortcut {
            sequence: "Ctrl+1"
            name: "mode.view"
        }

        EventShortcut {
            sequence: "Ctrl+2"
            name: "mode.analyse"
        }

        EventShortcut {
            sequence: "Ctrl+3"
            name: "mode.edit"
        }

        EventShortcut {
            sequence: "Ctrl+4"
            name: "mode.examples"
        }

        EventShortcut {
            sequence: "1"
            name: "simulator.speed.normal"
        }

        EventShortcut {
            sequence: "2"
            name: "simulator.speed.fast"
        }

        EventShortcut {
            sequence: "3"
            name: "simulator.speed.faster"
        }
    }

    Settings {
        id: settings
        property alias width: applicationRoot.width
        property alias height: applicationRoot.height
        property alias renderQuality: mainDesktop.renderQuality
        property alias backgroundColor: mainDesktop.backgroundColor
    }

    function resetStyle() {
        Style.reset(width, height, Screen)
    }

    onWidthChanged: resetStyle()

    onHeightChanged:resetStyle()

    Component.onCompleted: {
        resetStyle()
        mainDesktop.visualizer.mainCompleted = true
    }

    MainDesktop {
        id: mainDesktop
        initialScriptFilePath: applicationRoot.initialScriptFilePath
        anchors.fill: parent
        mouseMover: MouseMover {
            id: mouseMover
            window: applicationRoot
        }
    }
}
