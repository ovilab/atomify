import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import QtQuick.Window 2.2
import Atomify 1.0
import QtCharts 2.1
import QtQuick.Controls.Styles 1.4
import QtDataVisualization 1.2

WindowGL2 {
    id: root
    width: 500
    height: 500

//    property var dataSeries: []
    property Fix fix

    onFixChanged: {
        surfaceSeries.dataProxy = fix.data[0]
        fix.data[0].updated.connect(function() {
            xAxis.min = fix.data[0].xMin
            xAxis.max = fix.data[0].xMax
            yAxis.min = fix.data[0].yMin
            yAxis.max = fix.data[0].yMax
            zAxis.min = fix.data[0].zMin
            zAxis.max = fix.data[0].zMax
        })
    }

    Shortcut {
        sequence: StandardKey.Close
        onActivated: {
            root.close()
        }
    }

    Rectangle {
        id: surfaceView
        anchors.fill: parent

        ColorGradient {
            id: surfaceGradient
            ColorGradientStop { position: 0.0; color: "black" }
            ColorGradientStop { position: 0.2; color: "red" }
            ColorGradientStop { position: 0.5; color: "blue" }
            ColorGradientStop { position: 0.8; color: "yellow" }
            ColorGradientStop { position: 1.0; color: "white" }
        }

        ValueAxis3D {
            id: xAxis
            segmentCount: 8
            labelFormat: "%f"
            title: "x"
            min: 0
            max: 17
            titleVisible: true
            titleFixed: false
        }

        ValueAxis3D {
            id: yAxis
            segmentCount: 8
            labelFormat: "%f"
            title: "Value"
            min: 0
            max: 10
            titleVisible: true
            labelAutoRotation: 0
            titleFixed: false
        }

        ValueAxis3D {
            id: zAxis
            segmentCount: 5
            labelFormat: "%f"
            title: "z"
            min: 0
            max: 17
            titleVisible: true
            titleFixed: false
        }

        Theme3D {
            id: customTheme
            type: Theme3D.ThemeQt
            // Don't show specular spotlight as we don't want it to distort the colors
            lightStrength: 0.0
            ambientLightStrength: 1.0
            backgroundEnabled: false
            gridLineColor: "#AAAAAA"
            windowColor: "#EEEEEE"
        }

        Surface3D {
            //! [7]
            id: surfaceGraph
            width: Math.min(parent.width, parent.height)
            height: Math.min(parent.width, parent.height)
            onWidthChanged: {
                console.log("Width: ", width)
            }
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top

            // shadowQuality: AbstractGraph3D.ShadowQualityHigh
            // selectionMode: AbstractGraph3D.SelectionSlice | AbstractGraph3D.SelectionItemAndColumn
            axisX: xAxis
            axisY: yAxis
            axisZ: zAxis

            theme: customTheme
            inputHandler: TouchInputHandler3D {
                id: customInputHandler
//                rotationEnabled: false
//                zoomEnabled: false
            }
            orthoProjection: true
            scene.activeCamera.cameraPreset:  Camera3D.CameraPresetDirectlyAbove
            Component.onCompleted: {
                scene.activeCamera.zoomLevel = 110
            }
            flipHorizontalGrid: true
            radialLabelOffset: 0.01

            horizontalAspectRatio: 1
            scene.activeCamera.zoomLevel: 85

            Surface3DSeries {
                id: surfaceSeries
                flatShadingEnabled: false
                drawMode: Surface3DSeries.DrawSurface
                baseGradient: surfaceGradient
                colorStyle: Theme3D.ColorStyleRangeGradient
                //  itemLabelFormat: "(@xLabel, @zLabel): @yLabel"
            }
        }
    }

    Rectangle {
        id: legend
        anchors.margins: 20
        anchors.bottom: parent.bottom
        anchors.top: parent.top
        anchors.right: parent.right
        border.color: "black"
        border.width: 1
        width: 50
        rotation: 180
        gradient: Gradient {
            GradientStop { position: 0.0; color: "black" }
            GradientStop { position: 0.2; color: "red" }
            GradientStop { position: 0.5; color: "blue" }
            GradientStop { position: 0.8; color: "yellow" }
            GradientStop { position: 1.0; color: "white" }
        }
    }

    Text {
        anchors.verticalCenter: legend.bottom
        anchors.right: legend.left
        anchors.margins: 2
        text: surfaceGraph.axisY.min
    }

    Text {
        anchors.verticalCenter: legend.verticalCenter
        anchors.right: legend.left
        anchors.margins: 2
        text: (surfaceGraph.axisY.max + surfaceGraph.axisY.min) / 2
    }

    Text {
        anchors.verticalCenter: legend.top
        anchors.right: legend.left
        anchors.margins: 2
        text: surfaceGraph.axisY.max
    }
}
