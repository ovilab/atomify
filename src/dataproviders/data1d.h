#ifndef DATA1D_H
#define DATA1D_H
#include <limits>
#include <QObject>
#include <QXYSeries>
#include <QLineSeries>
#include <QMutex>

using namespace QtCharts;
class Data1D : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(qreal xMin READ xMin NOTIFY xMinChanged)
    Q_PROPERTY(qreal xMax READ xMax NOTIFY xMaxChanged)
    Q_PROPERTY(qreal yMin READ yMin NOTIFY yMinChanged)
    Q_PROPERTY(qreal yMax READ yMax NOTIFY yMaxChanged)
    Q_PROPERTY(QXYSeries* xySeries READ xySeries WRITE setXySeries NOTIFY xySeriesChanged)
    Q_PROPERTY(QString label READ label WRITE setLabel NOTIFY labelChanged)
    Q_PROPERTY(int bins READ bins WRITE setBins NOTIFY binsChanged)

public:
    explicit Data1D(QObject *parent = 0);
    Q_INVOKABLE void updateLimits();
    Q_INVOKABLE void updateXYSeries(QAbstractSeries *series);
    Q_INVOKABLE void updateHistogram(QLineSeries *series);
    Q_INVOKABLE void add(float x, float y, bool silent = true);
    Q_INVOKABLE void clear(bool silent = false);
    void createHistogram(const std::vector<double> &points);
    void add(const QPointF &point, bool silent = true);
    qreal xMin();
    qreal xMax();
    qreal yMin();
    qreal yMax();
    bool enabled() const;
    const QVector<QPointF> &points();
    QXYSeries* xySeries() const;
    void copyHistogram(const QVector<QPointF> &points);
    QString label() const;
    int bins() const;
signals:
    void xMinChanged(qreal xMin);
    void xMaxChanged(qreal xMax);
    void yMinChanged(qreal yMin);
    void yMaxChanged(qreal yMax);
    void minMaxChanged();
    void enabledChanged(bool enabled);
    void xySeriesChanged(QXYSeries* xySeries);
    void updated();
    void updatedHistogram(Data1D *data);
    void labelChanged(QString label);
    void binsChanged(int bins);

public slots:
    void setEnabled(bool enabled);
    void setXySeries(QXYSeries* xySeries);
    void setLabel(QString label);
    void setBins(int bins);

private:
    QXYSeries* m_xySeries = nullptr;
    QVector<QPointF> m_points;
    std::vector<double> m_histogramPoints;
    std::vector<double> m_cleanHistogramPoints; // without inf and NaN
    qreal m_xMin = 0;
    qreal m_xMax = 0;
    qreal m_yMin = 0;
    qreal m_yMax = 0;
    bool m_minMaxValuesDirty = false;
    bool m_enabled = false;
    void updateMinMaxWithPoint(const QPointF &point);
    QMutex m_mutex;
    QString m_label;
    int m_bins = 20;
};

#endif // DATA1D_H
