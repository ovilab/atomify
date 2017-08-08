import QtQuick 2.7
import QtQuick.Controls 1.5
import QtQuick.Layouts 1.0
import QtQuick.Window 2.2
import Atomify 1.0
import QtCharts 2.1
import QtQuick.Controls.Styles 1.4
import QtDataVisualization 1.2

WindowGL2 {
    id: root
    width: 850
    height: 750

    property Fix fix

    function updateLimits() {
        xAxis.min = fix.data[0].xMin
        xAxis.max = fix.data[0].xMax
        yAxis.min = fix.data[0].yMin
        yAxis.max = fix.data[0].yMax
        zAxis.min = fix.data[0].zMin
        zAxis.max = fix.data[0].zMax
    }

    onFixChanged: {
        surfaceSeries.dataProxy = fix.data[0]
        fix.data[0].updated.connect(updateLimits)
        updateLimits()
    }

    Shortcut {
        sequence: StandardKey.Close
        onActivated: {
            root.close()
        }
    }

    ColorMaps {
        id: colorMaps
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
        title: "y"
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
        colorStyle: Theme3D.ColorStyleRangeGradient
        font.pointSize: 30
        gridEnabled: false // TODO: checkbox
        gridLineColor: "#AAAAAA"
        windowColor: root.color
    }

    Surface3D {
        id: surfaceGraph
        anchors {
            left: parent.left
            top: parent.top
            bottom: parent.bottom
            right: legendMax.left
            topMargin: -100
            rightMargin: 50
        }

        shadowQuality: AbstractGraph3D.ShadowQualityNone
        axisX: xAxis
        axisY: yAxis
        axisZ: zAxis

        theme: customTheme
        // theme: Theme3D.ThemeDigia
        inputHandler: TouchInputHandler3D {
            id: customInputHandler
            rotationEnabled: true
            zoomEnabled: false
        }
        orthoProjection: true
        scene.activeCamera.cameraPreset:  Camera3D.CameraPresetDirectlyAbove
        flipHorizontalGrid: true
        radialLabelOffset: 0.01
        // selectionMode: AbstractGraph3D.SelectionSlice | AbstractGraph3D.SelectionItemAndRow
        selectionMode: AbstractGraph3D.SelectionItem
        horizontalAspectRatio: 1
        scene.activeCamera.zoomLevel: 100

        Surface3DSeries {
            id: surfaceSeries
            flatShadingEnabled: true
            drawMode: Surface3DSeries.DrawSurface
            baseGradient: colorMaps.inferno
            colorStyle: Theme3D.ColorStyleRangeGradient
            itemLabelFormat: "(@xLabel, @zLabel): @yLabel"
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
        gradient: colorMaps.inferno_
    }

    Label {
        id: legendMax
        anchors.verticalCenter: legend.top
        anchors.right: legend.left
        anchors.margins: 10
        width: 30
        horizontalAlignment: Qt.AlignRight
        color: "black"
        text: surfaceGraph.axisY.max.toFixed(2)
    }

    Label {
        anchors.verticalCenter: legend.verticalCenter
        anchors.right: legend.left
        anchors.margins: 10
        width: legendMax.width
        horizontalAlignment: Qt.AlignRight
        color: "black"
        text: ((surfaceGraph.axisY.max + surfaceGraph.axisY.min) / 2).toFixed(2)
    }

    Label {
        anchors.verticalCenter: legend.bottom
        anchors.right: legend.left
        anchors.margins: 10
        width: legendMax.width
        horizontalAlignment: Qt.AlignRight
        color: "black"
        text: surfaceGraph.axisY.min.toFixed(2)
    }

    Row {
        spacing: 3
        Label {
            y: 5
            text: "Colormap: "
        }

        ComboBox {
            model: ["Viridis", "Magma", "Inferno", "Plasma" ]
            currentIndex: 0
            onCurrentTextChanged: {
                if(currentText==="Magma") {
                    legend.gradient = colorMaps.magma_
                    surfaceSeries.baseGradient = colorMaps.magma
                } else if(currentText==="Inferni") {
                    legend.gradient = colorMaps.inferno_
                    surfaceSeries.baseGradient = colorMaps.inferno
                } else if(currentText==="Viridis") {
                    legend.gradient = colorMaps.viridis_
                    surfaceSeries.baseGradient = colorMaps.viridis
                } else if(currentText==="Plasma") {
                    legend.gradient = colorMaps.plasma_
                    surfaceSeries.baseGradient = colorMaps.plasma
                }
            }
        }
    }
}




