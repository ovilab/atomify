#include "data1d.h"
#include <QLineSeries>
#include <limits>
#include <QDebug>
#include <cmath>
#include <QMutexLocker>
#include <QFile>

Data1D::Data1D(QObject *parent) : QObject(parent)
{

}

qreal Data1D::xMin()
{
    return m_xMin;
}

qreal Data1D::xMax()
{
    return m_xMax;
}

qreal Data1D::yMin()
{
    return m_yMin;
}

qreal Data1D::yMax()
{
    return m_yMax;
}

void Data1D::updateLimits()
{
    QMutexLocker locker(&m_mutex);
    if(!m_minMaxValuesDirty) return;

    if(m_points.size() == 0) {
        m_minMaxValuesDirty = false;
        return;
    }

    // Assume that max/min values are in first value, test for all other values. This is guaranteed to be correct.
    qreal xMax = m_points.first().x();
    qreal xMin = m_points.first().x();
    qreal yMax = m_points.first().y();
    qreal yMin = m_points.first().y();

    for(const QPointF &p : m_points) {
        xMax = std::max(p.x(), xMax);
        xMin = std::min(p.x(), xMin);
        yMax = std::max(p.y(), yMax);
        yMin = std::min(p.y(), yMin);
    }
    m_minMaxValuesDirty = false; // in case signals below trigger new calculation

    bool anyChanges = false;
    if(!qFuzzyCompare(xMax,m_xMax)) {
        m_xMax = xMax;
        emit xMaxChanged(m_xMax);
        anyChanges = true;
    }

    if(!qFuzzyCompare(xMin, m_xMin)) {
        m_xMin = xMin;
        emit xMinChanged(m_xMin);
        anyChanges = true;
    }

    if(!qFuzzyCompare(yMax, m_yMax)) {
        m_yMax = yMax;
        emit yMaxChanged(m_yMax);
        anyChanges = true;
    }

    if(!qFuzzyCompare(yMin, m_yMin)) {
        m_yMin = yMin;
        emit yMinChanged(m_yMin);
        anyChanges = true;
    }

    if(anyChanges) {
        emit minMaxChanged();
    }

}

void Data1D::updateXYSeries(QAbstractSeries *series)
{
    QXYSeries *xySeries = qobject_cast<QXYSeries*>(series);
    if(xySeries) {
        xySeries->replace(m_points);
    }
}

void Data1D::copyHistogram(const QVector<QPointF> &points)
{
    if(points.size()<2) return;

    float dx = points[1].x() - points[0].x();
    setBins(points.size());
    float maxValue = points.first().y();
    float xMin = points.first().x()-0.5*dx;
    float xMax = points.last().x()+0.5*dx;

    m_points.clear();
    for(int bin = 0; bin<m_bins; bin++) {
        float binMin = points.first().x() + (bin-0.5)*dx;
        float binMax = points.first().x() + (bin+0.5)*dx;
        float value = points.at(bin).y();
        maxValue = std::max(maxValue, value);

        if(bin==0) {
            m_points.append(QPointF(binMin, 0));
        }

        m_points.append(QPointF(binMin, value));
        m_points.append(QPointF(binMax, value));
        m_points.append(QPointF(binMax, 0));
    }

    m_xMin = xMin;
    m_xMax = xMax;
    m_yMin = 0;
    m_yMax = maxValue*1.1;
}

QXYSeries *Data1D::xySeries() const
{
    return m_xySeries;
}

bool Data1D::enabled() const
{
    return m_enabled;
}

const QVector<QPointF> &Data1D::points()
{
    return m_points;
}

void Data1D::clear(bool silent)
{
    QMutexLocker locker(&m_mutex);
    m_minMaxValuesDirty = true;
    m_points.clear();
    m_cleanHistogramPoints.clear();
    if(!silent && m_xySeries) {
        updateXYSeries(m_xySeries);
    }
}

void Data1D::createHistogram(const std::vector<double> &points)
{
    QMutexLocker locker(&m_mutex);

    m_cleanHistogramPoints.clear();
    m_cleanHistogramPoints.reserve(points.size());
    for(double p : points) {
        if(std::isnan(p) || std::isinf(p)) continue;
        m_cleanHistogramPoints.push_back(p);
    }

    double min = *std::min_element(std::begin(m_cleanHistogramPoints), std::end(m_cleanHistogramPoints));
    double max = *std::max_element(std::begin(m_cleanHistogramPoints), std::end(m_cleanHistogramPoints));

    if(max == min) {
        // Ensure dx is not zero
        max = min+1;
    }
    double dx = (max-min) / m_bins;
    std::vector<int> counts(m_bins, 0);
    for(double p : m_cleanHistogramPoints) {
        int bin = (p-min) / dx;
        if(bin >= m_bins) bin = m_bins-1; // The very last number is exactly on the edge, put it in last bin
        counts[bin]++;
    }

    m_points.clear();
    m_points.reserve(3*m_bins+1);
    m_points.append(QPointF(min, 0));
    double maxCount = 0;
    for(int bin = 0; bin<m_bins; bin++) {
        double binMin = min + bin*dx;
        double binMax = min + (bin+1)*dx;
        double value = counts[bin];
        maxCount = std::max(maxCount, value);

        m_points.append(QPointF(binMin, value));
        m_points.append(QPointF(binMax, value));
        m_points.append(QPointF(binMax, 0));
    }
    counts.clear();
    m_xMin = min;
    m_xMax = max;
    m_yMin = 0;
    m_yMax = maxCount*1.1;
}

void Data1D::add(float x, float y, bool silent)
{
    add(QPointF(x,y), silent);
}

void Data1D::add(const QPointF &point, bool silent)
{
    QMutexLocker locker(&m_mutex);
    m_points.append(point);
    m_minMaxValuesDirty = true;
    if(!silent && m_xySeries) {
        updateMinMaxWithPoint(point);
        m_xySeries->append(point.x(), point.y());
    }
}

void Data1D::updateMinMaxWithPoint(const QPointF &point) {
    m_minMaxValuesDirty = false;
    bool singlePoint = m_points.size() == 1;

    bool anyChanges = false;
    if(m_xMax < point.x() || singlePoint) {
        m_xMax = point.x();
        emit xMaxChanged(m_xMax);
        anyChanges = true;
    }

    if(m_xMin > point.x() || singlePoint) {
        m_xMin = point.x();
        emit xMinChanged(m_xMin);
        anyChanges = true;
    }

    if(m_yMax < point.y() || singlePoint) {
        m_yMax = point.y();
        emit yMaxChanged(m_yMax);
        anyChanges = true;
    }

    if(m_yMin > point.y() || singlePoint) {
        m_yMin = point.y();
        emit yMinChanged(m_yMin);
        anyChanges = true;
    }

    if(anyChanges) {
        emit minMaxChanged();
    }
}

void Data1D::setEnabled(bool enabled)
{
    if (m_enabled == enabled)
        return;

    m_enabled = enabled;
    emit enabledChanged(enabled);
}

void Data1D::setXySeries(QXYSeries *xySeries)
{
    if (m_xySeries == xySeries)
        return;

    m_xySeries = xySeries;
    emit xySeriesChanged(xySeries);
}

void Data1D::setLabel(QString label)
{
    if (m_label == label)
            return;

        m_label = label;
        emit labelChanged(m_label);
}

void Data1D::setBins(int bins)
{
    if (m_bins == bins)
            return;

        m_bins = bins;
        emit binsChanged(m_bins);
}

QString Data1D::label() const
{
    return m_label;
}

int Data1D::bins() const
{
    return m_bins;
}
