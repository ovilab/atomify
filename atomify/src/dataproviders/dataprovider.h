#ifndef DATAPROVIDER_H
#define DATAPROVIDER_H

#include <QObject>
#include "data1d.h"

class DataProvider : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool hasScalarData READ hasScalarData WRITE setHasScalarData NOTIFY hasScalarDataChanged)
    Q_PROPERTY(float scalarValue READ scalarValue WRITE setScalarValue NOTIFY scalarValueChanged)
    Q_PROPERTY(int num1DData READ num1DData WRITE setNum1DData NOTIFY num1DDataChanged)
    Q_PROPERTY(QVariantMap data1D READ data1D WRITE setData1D NOTIFY data1DChanged)
public:
    explicit DataProvider(QObject *parent = 0);
    bool hasScalarData() const;
    int num1DData() const;
    QVariantMap data1D() const;
    float scalarValue() const;

signals:
    void hasScalarDataChanged(bool hasScalarData);
    void num1DDataChanged(int num1DData);
    void data1DChanged(QVariantMap data1D);
    void scalarValueChanged(float scalarValue);

public slots:
    void setHasScalarData(bool hasScalarData);
    void setNum1DData(int num1DData);
    void setData1D(QVariantMap data1D);
    void setScalarValue(float scalarValue);

protected:
    bool m_hasScalarData = false;
    int m_num1DData = 0;
    float m_scalarValue = 0;
    QVariantMap m_data1D;
    QMap<QString, Data1D*> m_data1DRaw;
};

#endif // DATAPROVIDER_H
