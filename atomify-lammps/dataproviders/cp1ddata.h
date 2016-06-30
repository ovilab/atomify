#ifndef CP1DDATA_H
#define CP1DDATA_H

#include <QObject>
#include <QXYSeries>
using namespace QtCharts;
class CP1DData : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(float xMin READ xMin WRITE setXMin NOTIFY xMinChanged)
    Q_PROPERTY(float xMax READ xMax WRITE setXMax NOTIFY xMaxChanged)
    Q_PROPERTY(float yMin READ yMin WRITE setYMin NOTIFY yMinChanged)
    Q_PROPERTY(float yMax READ yMax WRITE setYMax NOTIFY yMaxChanged)

public:
    explicit CP1DData(QObject *parent = 0);
    void clear();
    void add(float x, float y, bool silent = false);
    float xMin() const;
    float xMax() const;
    float yMin() const;
    float yMax() const;
    Q_INVOKABLE void updateLimits();
    Q_INVOKABLE void updateData(QAbstractSeries *series);
    bool enabled() const;

signals:
    void xMinChanged(float xMin);
    void xMaxChanged(float xMax);
    void yMinChanged(float yMin);
    void yMaxChanged(float yMax);
    void updated();
    void enabledChanged(bool enabled);

public slots:
    void setXMin(float xMin);
    void setXMax(float xMax);
    void setYMin(float yMin);
    void setYMax(float yMax);
    void setEnabled(bool enabled);

private:
    float m_xMin = 0;
    float m_xMax = 0;
    float m_yMin = 0;
    float m_yMax = 0;
    QVector<QPointF> m_points;
    bool m_enabled = false;
};

#endif // CP1DDATA_H
