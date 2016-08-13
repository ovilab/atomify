import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import QtQuick.Window 2.2
import Atomify 1.0
import QtCharts 2.1
import QtQuick.Controls.Styles 1.4
import QtDataVisualization 1.2

WindowGL2 {
    id: mainview
    width: 500
    height: 500

//    property var dataSeries: []
    property Fix fix

    onFixChanged: {
        surfaceSeries.dataProxy = fix.data[0]
//        xAxis.min = fix.data[0].xMin
//        xAxis.max = fix.data[0].xMax

//        yAxis.min = fix.data[0].yMin
//        yAxis.max = fix.data[0].yMax

//        zAxis.min = fix.data[0].zMin
//        zAxis.max = fix.data[0].zMax
//        for(var key in fix.data) {
//            console.log("Stuff: ", fix.data[key])

//            // compute.data1D[key].updated.connect(updateGraphs(key))
//        }
        //title = "Compute '"+compute.identifier+"'"
    }

    Shortcut {
        sequence: StandardKey.Close
        onActivated: {
            root.close()
        }
    }

    Item {
        id: surfaceView
        width: mainview.width
        height: mainview.height
        anchors.top: mainview.top
        anchors.left: mainview.left

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
            labelFormat: "%i\u00B0"
            title: "Angle"
            min: 0
            max: 17
            titleVisible: true
            titleFixed: false
        }

        ValueAxis3D {
            id: yAxis
            segmentCount: 8
            labelFormat: "%i \%"
            title: "Value"
            min: -2
            max: 2
            titleVisible: true
            labelAutoRotation: 0
            titleFixed: false
        }

        ValueAxis3D {
            id: zAxis
            segmentCount: 5
            labelFormat: "%i nm"
            title: "Radius"
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
            width: surfaceView.width
            height: surfaceView.height

            shadowQuality: AbstractGraph3D.ShadowQualityHigh
            // selectionMode: AbstractGraph3D.SelectionSlice | AbstractGraph3D.SelectionItemAndColumn
            axisX: xAxis
            axisY: yAxis
            axisZ: zAxis

            theme: customTheme
            //! [6]
            // inputHandler: customInputHandler
            //! [6]

            // Remove the perspective and view the graph from top down to achieve 2D effect
            //! [1]
            orthoProjection: true
            scene.activeCamera.cameraPreset: Camera3D.CameraPresetFront
            //! [1]

            //! [2]
            flipHorizontalGrid: true
            //! [2]

            //! [4]
            radialLabelOffset: 0.01
            //! [4]

            horizontalAspectRatio: 1
            scene.activeCamera.zoomLevel: 85

            Surface3DSeries {
                id: surfaceSeries
                flatShadingEnabled: false
                drawMode: Surface3DSeries.DrawSurface
                baseGradient: surfaceGradient
                colorStyle: Theme3D.ColorStyleRangeGradient
                itemLabelFormat: "(@xLabel, @zLabel): @yLabel"
            }
        }
    }
}
