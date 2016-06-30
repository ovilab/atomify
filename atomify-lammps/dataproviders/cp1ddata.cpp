#include "cp1ddata.h"
#include <QLineSeries>
CP1DData::CP1DData(QObject *parent) : QObject(parent)
{

}

float CP1DData::xMin() const
{
    return m_xMin;
}

float CP1DData::xMax() const
{
    return m_xMax;
}

float CP1DData::yMin() const
{
    return m_yMin;
}

float CP1DData::yMax() const
{
    return m_yMax;
}

void CP1DData::updateLimits()
{
    qreal xMax = -1e9;
    qreal xMin = 1e9;
    qreal yMax = -1e9;
    qreal yMin = 1e9;
    for(QPointF &p : m_points) {
        xMax = std::max(p.x(), xMax);
        xMin = std::min(p.x(), xMin);
        yMax = std::max(p.y(), yMax);
        yMin = std::min(p.y(), yMin);
    }
    setXMax(xMax);
    setXMin(xMin);
    setYMax(yMax);
    setYMin(yMin);
}

void CP1DData::updateData(QAbstractSeries *series)
{
    if(series) {
        QXYSeries *xySeries = static_cast<QXYSeries*>(series);
        xySeries->replace(m_points);
    }
}

bool CP1DData::enabled() const
{
    return m_enabled;
}

void CP1DData::clear()
{
    m_points.clear();
}

void CP1DData::add(float x, float y, bool silent)
{
    m_points.append(QPointF(x,y));
    if(!silent) emit updated();
}

void CP1DData::setXMin(float xMin)
{
    if (m_xMin == xMin)
        return;

    m_xMin = xMin;
    emit xMinChanged(xMin);
}

void CP1DData::setXMax(float xMax)
{
    if (m_xMax == xMax)
        return;

    m_xMax = xMax;
    emit xMaxChanged(xMax);
}

void CP1DData::setYMin(float yMin)
{
    if (m_yMin == yMin)
        return;

    m_yMin = yMin;
    emit yMinChanged(yMin);
}

void CP1DData::setYMax(float yMax)
{
    if (m_yMax == yMax)
        return;

    m_yMax = yMax;
    emit yMaxChanged(yMax);
}

void CP1DData::setEnabled(bool enabled)
{
    if (m_enabled == enabled)
        return;

    m_enabled = enabled;
    emit enabledChanged(enabled);
}
