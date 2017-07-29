#ifndef DATASOURCE_H
#define DATASOURCE_H
#include <QObject>
#include <QVector>
#include <QVariantList>
#include <QPointF>
class DataSource : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantList xValues READ xValues WRITE setXValues NOTIFY xValuesChanged)
    Q_PROPERTY(QVariantList yValues READ yValues WRITE setYValues NOTIFY yValuesChanged)

private:
    QVariantList m_xValues;
    QVariantList m_yValues;
    QVector<float> m_xValuesRaw;
    QVector<float> m_yValuesRaw;
    bool isValid();

public:
    DataSource();
    QVariantList xValues() const;
    QVariantList yValues() const;
    Q_INVOKABLE float xMin();
    Q_INVOKABLE float xMax();
    Q_INVOKABLE float yMin();
    Q_INVOKABLE float yMax();
    Q_INVOKABLE void setPoints(QVector<QPointF> points, bool normalized = false);
    Q_INVOKABLE void addPoint(float x, float y);
    Q_INVOKABLE void normalizeArea();
    Q_INVOKABLE void clear();
    void update();
public slots:
    void setXValues(QVariantList xValues);
    void setYValues(QVariantList yValues);

signals:
    void xValuesChanged(QVariantList xValues);
    void yValuesChanged(QVariantList yValues);
    void updated();
};

#endif // DATASOURCE_H
