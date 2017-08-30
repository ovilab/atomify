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
    Q_PROPERTY(float xMin READ xMin NOTIFY xMinChanged)
    Q_PROPERTY(float xMax READ xMax NOTIFY xMaxChanged)
    Q_PROPERTY(float yMin READ yMin NOTIFY yMinChanged)
    Q_PROPERTY(float yMax READ yMax NOTIFY yMaxChanged)
    Q_PROPERTY(QXYSeries* xySeries READ xySeries WRITE setXySeries NOTIFY xySeriesChanged)
    Q_PROPERTY(QString label READ label WRITE setLabel NOTIFY labelChanged)

public:
    explicit Data1D(QObject *parent = 0);
    Q_INVOKABLE void updateLimits();
    Q_INVOKABLE void updateXYSeries(QAbstractSeries *series);
    Q_INVOKABLE void updateHistogram(QLineSeries *series);
    Q_INVOKABLE void add(float x, float y, bool silent = true);
    Q_INVOKABLE void clear(bool silent = false);
    void createHistogram(const std::vector<double> &points);
    void add(const QPointF &point, bool silent = true);
    float xMin();
    float xMax();
    float yMin();
    float yMax();
    bool enabled() const;
    const QList<QPointF> &points();
    QXYSeries* xySeries() const;
    bool isHistogram() const;
    void setIsHistogram(bool isHistogram);
    QString label() const;

signals:
    void xMinChanged(float xMin);
    void xMaxChanged(float xMax);
    void yMinChanged(float yMin);
    void yMaxChanged(float yMax);
    void minMaxChanged();
    void enabledChanged(bool enabled);
    void xySeriesChanged(QXYSeries* xySeries);
    void updated();
    void updatedHistogram(Data1D *data);
    void labelChanged(QString label);

public slots:
    void setEnabled(bool enabled);
    void setXySeries(QXYSeries* xySeries);
    void setLabel(QString label);

private:
    QXYSeries* m_xySeries = nullptr;
    QList<QPointF> m_points;
    std::vector<double> m_histogramPoints;
    std::vector<double> m_cleanHistogramPoints;
    float m_xMin = 0;
    float m_xMax = 0;
    float m_yMin = 0;
    float m_yMax = 0;
    bool m_minMaxValuesDirty = false;
    bool m_enabled = false;
    bool m_isHistogram = false;
    void updateMinMaxWithPoint(const QPointF &point);
    QMutex m_mutex;
    QString m_label;
};

#endif // DATA1D_H
