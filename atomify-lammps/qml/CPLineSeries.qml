import QtQuick 2.6
import QtCharts 2.1
import Atomify 1.0

LineSeries {
    id: lineSeriesRoot
    property DataSource dataSource

    function updateData() {
        clear()
        if(dataSource === undefined) {
            return;
        }

        for(var i in dataSource.xValues) {
            var x = dataSource.xValues[i]
            var y = dataSource.yValues[i]
            append(x,y)
        }
    }

    onDataSourceChanged: {
        dataSource.updated.connect(updateData)
        updateData()
    }
}
