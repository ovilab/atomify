#include "dataprovider.h"

DataProvider::DataProvider(QObject *parent) : QObject(parent)
{

}

bool DataProvider::hasScalarData() const
{
    return m_hasScalarData;
}

int DataProvider::num1DData() const
{
    return m_num1DData;
}

QVariantMap DataProvider::data1D() const
{
    return m_data1D;
}

float DataProvider::scalarValue() const
{
    return m_scalarValue;
}

void DataProvider::setHasScalarData(bool hasScalarData)
{
    if (m_hasScalarData == hasScalarData)
        return;

    m_hasScalarData = hasScalarData;
    emit hasScalarDataChanged(hasScalarData);
    if(m_hasScalarData) {
        m_data1DRaw["scalar"] = new CP1DData(this);
    }
}

void DataProvider::setNum1DData(int num1DData)
{
    if (m_num1DData == num1DData)
        return;

    m_num1DData = num1DData;
    emit num1DDataChanged(num1DData);
}

void DataProvider::setData1D(QVariantMap data1D)
{
    if (m_data1D == data1D)
        return;

    m_data1D = data1D;
    emit data1DChanged(data1D);
}

void DataProvider::setScalarValue(float scalarValue)
{
    if (m_scalarValue == scalarValue)
        return;

    m_scalarValue = scalarValue;
    emit scalarValueChanged(scalarValue);
}
