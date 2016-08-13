#include "data2d.h"
#include <QDebug>
#include <QSurface3DSeries>

Data2D::Data2D(QObject *parent) : QSurfaceDataProxy(parent)
{
    m_dataArray = nullptr;
}

void Data2D::update() {

    resetArray(m_dataArray);
}

float Data2D::xMin() const
{
    return m_xMin;
}

float Data2D::yMin() const
{
    return m_yMin;
}

float Data2D::xMax() const
{
    return m_xMax;
}

float Data2D::zMin() const
{
    return m_zMin;
}

float Data2D::zMax() const
{
    return m_zMax;
}

float Data2D::yMax() const
{
    return m_yMax;
}

QSize Data2D::size() const
{
    return m_size;
}

void Data2D::setValue(float x, float y, float z)
{
    int i,j;
    if(plane==XY) {
        float deltaX = (m_xMax - m_xMin) / m_size.width();
        float deltaY = (m_yMax - m_yMin) / m_size.height();
        i = (x - m_xMin) / deltaX;
        j = (y - m_yMin) / deltaY;
    } else if(plane==XZ) {
        float deltaX = (m_xMax - m_xMin) / m_size.width();
        float deltaZ = (m_zMax - m_zMin) / m_size.height();
        i = (x - m_xMin) / deltaX;
        j = (z - m_zMin) / deltaZ;
    } else {
        float deltaY = (m_yMax - m_yMin) / m_size.width();
        float deltaZ = (m_zMax - m_zMin) / m_size.height();
        i = (y - m_yMin) / deltaY;
        j = (z - m_zMin) / deltaZ;
    }

    if(i < 0 || i >= m_size.width() || j < 0 || j >= m_size.height()) {
        qDebug() << "Warning, tried to set value outside range.";
        return;
    }
    try {
        QSurfaceDataRow &newRow = *m_dataArray->at(i);
        QSurfaceDataItem &item = newRow[j];
        item.setPosition(QVector3D(x,z,y));
    } catch(int ex) {
        qDebug() << "Went to hell...";
        exit(1);
    }
}


void Data2D::setSize(QSize size)
{
    if (m_size == size)
        return;

    m_size = size;
    if(m_dataArray) {
        // delete m_dataArray;
    }
    // Do not recreate array if dimensions have not changed
    m_dataArray = new QSurfaceDataArray;
    m_dataArray->reserve(size.height());
    for (int i = 0; i < size.height(); i++) {
        QSurfaceDataRow *newProxyRow = new QSurfaceDataRow(size.width());
        m_dataArray->append(newProxyRow);
    }
    resetArray(m_dataArray);

    emit sizeChanged(size);
}

void Data2D::setXMin(float xMin)
{
    if (m_xMin == xMin)
        return;

    m_xMin = xMin;
    emit xMinChanged(xMin);
}

void Data2D::setYMin(float yMin)
{
    if (m_yMin == yMin)
        return;

    m_yMin = yMin;
    emit yMinChanged(yMin);
}

void Data2D::setXMax(float xMax)
{
    if (m_xMax == xMax)
        return;

    m_xMax = xMax;
    emit xMaxChanged(xMax);
}

void Data2D::setZMin(float zMin)
{
    if (m_zMin == zMin)
        return;

    m_zMin = zMin;
    emit zMinChanged(zMin);
}

void Data2D::setZMax(float zMax)
{
    if (m_zMax == zMax)
        return;

    m_zMax = zMax;
    emit zMaxChanged(zMax);
}

void Data2D::setYMax(float yMax)
{
    if (m_yMax == yMax)
        return;

    m_yMax = yMax;
    emit yMaxChanged(yMax);
}

