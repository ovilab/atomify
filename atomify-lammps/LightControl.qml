import QtQuick 2.2
import QtQuick.Controls 1.1
import SimVis 1.0

Rectangle {
    id: lightControlRoot
    property Light light
    width: 320
    height: sliders.height + okButton.height + 10
    color: "#ffffff"
    opacity: 0.9

    onVisibleChanged: {
        focus = visible
    }

    GroupBox {
        id: sliders
        width: parent.width
        height: 142
        title: qsTr("Light")

        CheckBox {
            id: ambient
            text: "Ambient"
            checked: light.ambient
            onCheckedChanged: light.ambient = checked
        }

        Slider {
            id: ambientIntensity
            anchors.left: ambient.right
            anchors.leftMargin: 5
            maximumValue: 0.3
            minimumValue: 0
            value: light.ambientIntensity
            onValueChanged: {
                light.ambientIntensity = value
            }
        }

        CheckBox {
            id: diffuse
            width: ambient.width
            anchors.top: ambient.bottom
            text: "Diffuse"
            checked: light.diffuse
            onCheckedChanged: light.diffuse = checked
        }

        Slider {
            id: diffuseIntensity
            anchors.top: diffuse.top
            anchors.left: diffuse.right
            anchors.leftMargin: 5
            maximumValue: 1.0
            minimumValue: 0.0
            value: light.diffuseIntensity
            onValueChanged: {
                light.diffuseIntensity = value
            }
        }

        CheckBox {
            id: specular
            width: ambient.width
            anchors.top: diffuse.bottom
            text: "Specular"
            checked: light.specular
            onCheckedChanged: light.specular = checked
        }

        Slider {
            id: specularIntensity
            anchors.top: specular.top
            anchors.left: specular.right
            anchors.leftMargin: 5
            maximumValue: 1.0
            minimumValue: 0.0
            value: light.specularIntensity
            onValueChanged: light.specularIntensity = value
        }

        Label {
            id: shininess
            width: ambient.width
            anchors.top: specular.bottom
            text: "Shininess"
        }

        Slider {
            id: shininessIntensity
            anchors.top: shininess.top
            anchors.left: shininess.right
            anchors.leftMargin: 5
            height: 22
            maximumValue: 50
            minimumValue: 0.0
            value: (maximumValue+1) - light.shininess
            onValueChanged: light.shininess = (maximumValue+1) - value
        }

        Label {
            id: attenuation
            width: ambient.width
            anchors.top: shininess.bottom
            text: "Attenuation"
        }

        Slider {
            id: attenuationValue
            anchors.top: attenuation.top
            anchors.left: attenuation.right
            anchors.leftMargin: 5
            height: 22
            maximumValue: 50
            minimumValue: 0.0
            value: 1000.0*light.attenuation
            onValueChanged: light.attenuation = 0.001*value
        }

        Label {
            id: gamma
            width: ambient.width
            anchors.top: attenuation.bottom
            text: "Gamma"
        }

        Slider {
            id: gammaValue
            anchors.top: gamma.top
            anchors.left: gamma.right
            anchors.leftMargin: 5
            height: 22
            maximumValue: 3.0
            minimumValue: 0.0
            value: light.gamma
            onValueChanged: {
                if(light != null) light.gamma = value
            }
        }
    }

    Button {
        id: okButton
        x: parent.width*0.5 - width*0.5
        anchors.top: sliders.bottom
        anchors.topMargin: 5
        text: qsTr("OK")
        onClicked: lightControlRoot.visible = false
    }

    Keys.onPressed: {
        if(event.key === Qt.Key_Escape) {
            visible = false
        }
    }
}
