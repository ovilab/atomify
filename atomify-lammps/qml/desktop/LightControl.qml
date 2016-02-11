import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import SimVis 1.0

Item {
    id: lightControlRoot
    property Light light
    height: columnLayout.height
    width: parent.width
    property real labelWidth: 80
    ColumnLayout {
        id: columnLayout
        spacing: 5
        Row {
            spacing: 5
            CheckBox {
                id: ambient
                width: lightControlRoot.labelWidth
                text: "Ambient"
                checked: light ? light.ambient : false
                onCheckedChanged: {
                    light.ambient = checked
                }
            }

            Slider {
                id: ambientIntensity
                maximumValue: 1
                minimumValue: 0
                value: light ? light.ambientIntensity : 0
                onValueChanged: light.ambientIntensity = value
            }
        }
        Row {
            spacing: 5
            CheckBox {
                id: diffuse
                width: lightControlRoot.labelWidth
                text: "Diffuse"
                checked: light.diffuse
                onCheckedChanged: light.diffuse = checked
            }

            Slider {
                id: diffuseIntensity
                maximumValue: 1.0
                minimumValue: 0.0
                value: light.diffuseIntensity
                onValueChanged: light.diffuseIntensity = value
            }
        }
        Row {
            spacing: 5
            CheckBox {
                id: specular
                width: lightControlRoot.labelWidth
                text: "Specular"
                checked: light.specular
                onCheckedChanged: light.specular = checked
            }

            Slider {
                id: specularIntensity
                maximumValue: 3.0
                minimumValue: 0.0
                value: light.specularIntensity
                onValueChanged: light.specularIntensity = value
            }
        }

        Row {
            spacing: 5
            Label {
                id: shininess
                width: lightControlRoot.labelWidth
                text: "Shininess"
            }

            Slider {
                id: shininessIntensity
                maximumValue: 100
                minimumValue: 0.0
                value: light.shininess
                onValueChanged: light.shininess = value
//                value: (maximumValue+1) - light.shininess
//                onValueChanged: light.shininess = (maximumValue+1) - value
            }
        }

        Row {
            spacing: 5
            Label {
                id: attenuation
                width: lightControlRoot.labelWidth
                text: "Attenuation"
            }

            TextField {
                text: "0.0"
                inputMethodHints: Qt.ImhFormattedNumbersOnly
                onTextChanged: {
                    light.attenuation = parseFloat(text)
                }
            }
        }
    }


}
