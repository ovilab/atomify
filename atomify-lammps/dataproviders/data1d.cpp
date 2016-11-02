#include "data1d.h"
#include <QLineSeries>
#include <limits>
#include <QDebug>
#include <cmath>

Data1D::Data1D(QObject *parent) : QObject(parent)
{

}

float Data1D::xMin()
{
    return m_xMin;
}

float Data1D::xMax()
{
    return m_xMax;
}

float Data1D::yMin()
{
    return m_yMin;
}

float Data1D::yMax()
{
    return m_yMax;
}

void Data1D::updateLimits()
{
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

    for(QPointF &p : m_points) {
        xMax = std::max(p.x(), xMax);
        xMin = std::min(p.x(), xMin);
        yMax = std::max(p.y(), yMax);
        yMin = std::min(p.y(), yMin);
    }
    m_minMaxValuesDirty = false; // in case signals below trigger new calculation
    bool anyChanges = false;
    if(xMax != m_xMax) {
        m_xMax = xMax;
        emit xMaxChanged(m_xMax);
        anyChanges = true;
    }

    if(xMin != m_xMin) {
        m_xMin = xMin;
        emit xMinChanged(m_xMin);
        anyChanges = true;
    }

    if(yMax != m_yMax) {
        m_yMax = yMax;
        emit yMaxChanged(m_yMax);
        anyChanges = true;
    }

    if(yMin != m_yMin) {
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

void Data1D::updateHistogram(QLineSeries *series)
{
    m_cleanHistogramPoints.clear();
    m_cleanHistogramPoints.reserve(m_histogramPoints.size());
    for(double p : m_histogramPoints) {
        if(std::isnan(p) || std::isinf(p)) continue;
        m_cleanHistogramPoints.push_back(p);
    }

    double min = *std::min_element(std::begin(m_cleanHistogramPoints), std::end(m_cleanHistogramPoints));
    double max = *std::max_element(std::begin(m_cleanHistogramPoints), std::end(m_cleanHistogramPoints));

    int bins = 20; // TODO: be clever choosing this or allow user
    double dx = (max-min) / 20;
    std::vector<int> counts(bins, 0);
    for(double p : m_cleanHistogramPoints) {

        int bin = (p-min) / dx;
        if(bin >= bins) bin = bins-1; // The very last number is exactly on the edge, put it in last bin
        counts[bin]++;
    }

    QVector<QPointF> histogram;
    histogram.reserve(3*bins+1);
    histogram.append(QPointF(min, 0));
    double maxCount = 0;
    for(int bin = 0; bin<bins; bin++) {
        double binMin = min + bin*dx;
        double binMax = min + (bin+1)*dx;
        double value = counts[bin];
        maxCount = std::max(maxCount, value);

        histogram.append(QPointF(binMin, value));
        histogram.append(QPointF(binMax, value));
        histogram.append(QPointF(binMax, 0));
    }
    counts.clear();
    series->replace(histogram);
    histogram.clear();
    m_xMin = min;
    m_xMax = max;
    m_yMin = 0;
    m_yMax = maxCount*1.1;
    emit xMinChanged(m_xMin);
    emit xMaxChanged(m_xMax);
    emit yMinChanged(m_yMin);
    emit yMaxChanged(m_yMax);
    emit histogramPointsChanged(m_cleanHistogramPoints);
}

QVariantMap Data1D::subsets() const
{
    return m_subsets;
}

int Data1D::stride() const
{
    return m_stride;
}

Data1D *Data1D::parentData() const
{
    return m_parentData;
}

QXYSeries *Data1D::xySeries() const
{
    return m_xySeries;
}

QVector<double> Data1D::histogramPoints() const
{
    return m_cleanHistogramPoints;
}

void Data1D::addSubset(QString key, int stride)
{
    Data1D *data = new Data1D(this);
    data->setStride(stride);
    m_subsets.insert(key, QVariant::fromValue(data));
    data->setParentData(this);
    for(const QPointF &p : m_points) {
        data->add(p);
    }
}

bool Data1D::enabled() const
{
    return m_enabled;
}

void Data1D::clear(bool silent)
{
    m_minMaxValuesDirty = true;
    m_points.clear();
    m_strideCount = 0;
    for(QVariant &variant : m_subsets) {
        Data1D *data = variant.value<Data1D*>();
        data->clear();
    }
    if(m_xySeries) {
        updateXYSeries(m_xySeries);
    }
}

void Data1D::createHistogram(const std::vector<double> &points)
{
    m_histogramPoints = points;
    emit updatedHistogram(this);
}

void Data1D::add(float x, float y, bool silent)
{
    add(QPointF(x,y), silent);
}

void Data1D::add(const QPointF &point, bool silent)
{
    if(m_parentData) {
        m_strideCount++;

        if(m_points.size()==0 || m_strideCount >= m_stride) {
            // We should add this point and reset stride count if first point
            m_strideCount = 0;
        } else {
            return;
        }
    }

    m_points.append(point);
    m_minMaxValuesDirty = true;
    if(!silent && m_xySeries) {
        updateMinMaxWithPoint(point);
        m_xySeries->append(point.x(), point.y());
    }

    for(QVariant &variant : m_subsets) {
        Data1D *data = variant.value<Data1D*>();
        data->add(point, silent);
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

void Data1D::setSubsets(QVariantMap subsets)
{
    if (m_subsets == subsets)
        return;

    m_subsets = subsets;
    emit subsetsChanged(subsets);
}

void Data1D::setStride(int stride)
{
    if (m_stride == stride)
        return;

    m_stride = stride;
    emit strideChanged(stride);
}

void Data1D::setParentData(Data1D *parentData)
{
    if (m_parentData == parentData)
        return;

    m_parentData = parentData;
    emit parentDataChanged(parentData);
}

void Data1D::setXySeries(QXYSeries *xySeries)
{
    if (m_xySeries == xySeries)
        return;

    m_xySeries = xySeries;
    emit xySeriesChanged(xySeries);
}

void Data1D::setHistogramPoints(QVector<double> histogramPoints)
{
    if (m_cleanHistogramPoints == histogramPoints)
        return;

    m_cleanHistogramPoints = histogramPoints;
    emit histogramPointsChanged(histogramPoints);
}
