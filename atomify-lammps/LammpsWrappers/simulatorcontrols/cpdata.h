#ifndef CPDATA_H
#define CPDATA_H

#include <QObject>
#include <QXYSeries>
using namespace QtCharts;

class CPData : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool hasScalar READ hasScalar WRITE setHasScalar NOTIFY hasScalarChanged)
    Q_PROPERTY(bool hasVector READ hasVector WRITE setHasVector NOTIFY hasVectorChanged)
    Q_PROPERTY(bool hasArray READ hasArray WRITE setHasArray NOTIFY hasArrayChanged)
    Q_PROPERTY(QXYSeries* scalarValue READ scalarValue WRITE setScalarValue NOTIFY scalarValueChanged)

public:
    explicit CPData(QObject *parent = 0);
    bool hasScalar() const;
    bool hasVector() const;
    bool hasArray() const;
    QXYSeries* scalarValue() const;

signals:
    void hasScalarChanged(bool hasScalar);
    void hasVectorChanged(bool hasVector);
    void hasArrayChanged(bool hasArray);

    void scalarValueChanged(QXYSeries* scalarValue);

public slots:
    void setHasScalar(bool hasScalar);
    void setHasVector(bool hasVector);
    void setHasArray(bool hasArray);
    void setScalarValue(QXYSeries* scalarValue);

private:
    bool m_hasScalar = false;
    bool m_hasVector = false;
    bool m_hasArray = false;
    QXYSeries* m_scalarValue = nullptr;
};

#endif // CPDATA_H
