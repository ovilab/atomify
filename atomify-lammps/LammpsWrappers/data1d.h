#ifndef CP1DDATA_H
#define CP1DDATA_H

#include <QObject>
#include <QXYSeries>
using namespace QtCharts;
class Data1D : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(bool average READ average WRITE setAverage NOTIFY averageChanged)
    Q_PROPERTY(float xMin READ xMin WRITE setXMin NOTIFY xMinChanged)
    Q_PROPERTY(float xMax READ xMax WRITE setXMax NOTIFY xMaxChanged)
    Q_PROPERTY(float yMin READ yMin WRITE setYMin NOTIFY yMinChanged)
    Q_PROPERTY(float yMax READ yMax WRITE setYMax NOTIFY yMaxChanged)
    Q_PROPERTY(float xScale READ xScale WRITE setXScale NOTIFY xScaleChanged)
    Q_PROPERTY(float yScale READ yScale WRITE setYScale NOTIFY yScaleChanged)
    Q_PROPERTY(float xRange READ xRange WRITE setXRange NOTIFY xRangeChanged)
    Q_PROPERTY(float lastValue READ lastValue WRITE setLastValue NOTIFY lastValueChanged)
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
public:
    explicit Data1D(QObject *parent = 0);
    void clear();
    void add(float x, float y, bool silent = false);
    float xMin() const;
    float xMax() const;
    float yMin() const;
    float yMax() const;
    bool enabled() const;
    float xRange() const;
    bool pointIsValid(const QPointF &p);
    QList<QPointF> validPoints();
    Q_INVOKABLE void updateLimits();
    Q_INVOKABLE void updateData(QAbstractSeries *series);
    QString title() const;
    float xScale() const;
    float yScale() const;
    bool average() const;
    float lastValue() const;

signals:
    void xMinChanged(float xMin);
    void xMaxChanged(float xMax);
    void yMinChanged(float yMin);
    void yMaxChanged(float yMax);
    void updated();
    void enabledChanged(bool enabled);
    void xRangeChanged(float xRange);
    void titleChanged(QString title);
    void xScaleChanged(float xScale);
    void yScaleChanged(float yScale);
    void averageChanged(bool average);
    void lastValueChanged(float lastValue);

public slots:
    void setXMin(float xMin);
    void setXMax(float xMax);
    void setYMin(float yMin);
    void setYMax(float yMax);
    void setEnabled(bool enabled);
    void setXRange(float xRange);
    void setTitle(QString title);
    void setXScale(float xScale);
    void setYScale(float yScale);
    void setAverage(bool average);
    void setLastValue(float lastValue);

private:
    QList<QPointF> m_points;
    double m_sum = 0;
    int    m_numValues = 0;
    float m_xMin = 0;
    float m_xMax = 0;
    float m_yMin = 0;
    float m_yMax = 0;
    float m_xRange = 0;
    float m_xScale = 1.0;
    float m_yScale = 1.0;
    float m_lastValue = 0;
    bool m_enabled = false;
    bool m_average = false;
    QString m_title;
};

#endif // CP1DDATA_H
