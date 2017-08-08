import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import Atomify 1.0
import QtCharts 2.0
import "qrc:/mobile/style"
import "qrc:/mobile/dashboard"

ChartView {
    id: miniChart

    property alias axisX: _axisX
    property alias axisY: _axisY
    
    antialiasing: true
    legend.visible: false
    backgroundColor: Style.chart.backgroundColor

    margins {
        top: 0
        bottom: 0
        left: 0
        right: 0
    }
    
    ValueAxis {
        id: _axisX
        tickCount: 0
        labelsVisible: false
        gridVisible: false
        visible: false
    }

    ValueAxis {
        id: _axisY
        tickCount: 0
        labelsVisible: false
        gridVisible: false
        visible: false
    }

}
