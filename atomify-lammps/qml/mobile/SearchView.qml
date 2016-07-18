import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Window 2.0
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0
import Atomify 1.0
import SimVis 1.0
import Qt.labs.settings 1.0

import "qrc:/core"
import "qrc:/mobile/main-menu"
import "qrc:/mobile/dashboard"
import "qrc:/mobile/style"
import "qrc:/mobile/hud"
import "qrc:/mobile/simulations"
import "qrc:/visualization"

Item {
    id: root

    signal loadSimulation(var simulation)

    property var model

    anchors {
        top: parent.top
        left: parent.left
        right: parent.right
        bottom: parent.bottom
    }
    
    Component.onCompleted: {
        applyFilter()
    }

    onVisibleChanged: {
        if(visible) {
            searchField.forceActiveFocus()
            searchField.selectAll()
        }
    }
    
    function applyFilter() {
        searchModel.clear()
        for(var i in root.model) {
            var category = root.model[i]
            for(var j in category.simulations) {
                var simulation = category.simulations[j]
                if(searchField.text.length < 1 || simulation.indexOf(searchField.text) > -1) {
                    searchModel.append({name: simulation})
                }
            }
        }
    }
    
    Rectangle {
        anchors.fill: searchResultColumn
        color: "#aa000000"
    }
    
    TextField {
        id: searchField
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
        }
        onTextChanged: applyFilter()

        Keys.onReturnPressed: {
            loadSimulation(searchResultColumn.currentItem.simulation)
            root.visible = false
        }
        Keys.onDownPressed: {
            if(searchResultColumn.currentIndex === searchResultColumn.count - 1) {
                searchResultColumn.currentIndex = 0
            } else {
                searchResultColumn.currentIndex += 1
            }
        }
        Keys.onUpPressed: {
            if(searchResultColumn.currentIndex === 0) {
                searchResultColumn.currentIndex = searchResultColumn.count - 1
            } else {
                searchResultColumn.currentIndex -= 1
            }
        }
    }
    
    ListView {
        id: searchResultColumn
        anchors {
            top: searchField.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        delegate: Rectangle {
            id: simulationRectangle
            property alias simulation: simulationLoader.item
            color: searchResultColumn.currentIndex === index ? Style.color.foreground : Style.color.background
            width: parent.width
            height: Style.touchableSize
            Text {
                anchors.centerIn: parent
                text: simulationLoader.item.name
                color: Style.font.color
            }

            SimulationLoader {
                id: simulationLoader
                folder: name
            }
        }
        model: ListModel {
            id: searchModel
        }
    }
}
