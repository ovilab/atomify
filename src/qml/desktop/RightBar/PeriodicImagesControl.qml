import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Controls 1.5 as QQC1
import QtQuick.Dialogs 1.2
import Atomify 1.0
import Qt.labs.settings 1.0
import "../../visualization"

GroupBox {
    id: periodicImagesRoot
    property PeriodicImages periodicImages
    height: periodicImages.enabled ? undefined : label.height
    label: CheckBox {
        id: checkBox
        checked: periodicImages.enabled
        onCheckedChanged: periodicImages.enabled = checked
        text: qsTr("Periodic images")
        Binding {
            target: periodicImages
            property: "enabled"
            value: checkBox.checked
        }
    }
    Column {
        width: parent.width
        visible: periodicImagesRoot.periodicImages.enabled
        spacing: 10
        Row {
            width: parent.width
            height: periodicXSlider.height
            Label {
                width: parent.width*0.4
                text: "X: "+periodicXSlider.value
            }
            QQC1.Slider {
                id: periodicXSlider
                width: parent.width*0.6
                minimumValue: 1
                maximumValue: 5.0
                stepSize: 1
                value: periodicImages.numberOfCopiesX
                Binding {
                    target: periodicImages
                    property: "numberOfCopiesX"
                    value: periodicXSlider.value
                }
            }
        }
        
        Row {
            width: parent.width
            height: periodicYSlider.height
            Label {
                width: parent.width*0.4
                text: "Y: "+periodicYSlider.value
            }
            QQC1.Slider {
                id: periodicYSlider
                width: parent.width*0.6
                minimumValue: 1
                maximumValue: 5.0
                stepSize: 1
                value: periodicImages.numberOfCopiesY
                Binding {
                    target: visualizer.periodicImages
                    property: "numberOfCopiesY"
                    value: periodicYSlider.value
                }
            }
        }
        
        Row {
            width: parent.width
            height: periodicZSlider.height
            Label {
                width: parent.width*0.4
                text: "Z: "+periodicZSlider.value
            }
            QQC1.Slider {
                id: periodicZSlider
                width: parent.width*0.6
                minimumValue: 1
                maximumValue: 5.0
                stepSize: 1
                value: periodicImages.numberOfCopiesZ
                Binding {
                    target: visualizer.periodicImages
                    property: "numberOfCopiesZ"
                    value: periodicZSlider.value
                }
            }
        }
    }
}
