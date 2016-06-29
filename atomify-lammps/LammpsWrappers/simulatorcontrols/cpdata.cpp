#include "cpdata.h"
#include <QLineSeries>
#include <QScatterSeries>

CPData::CPData(QObject *parent) : QObject(parent)
{
    m_scalarValue = new QLineSeries(parent);
}

bool CPData::hasScalar() const
{
    return m_hasScalar;
}

bool CPData::hasVector() const
{
    return m_hasVector;
}

bool CPData::hasArray() const
{
    return m_hasArray;
}

QXYSeries *CPData::scalarValue() const
{
    return m_scalarValue;
}

void CPData::setHasScalar(bool hasScalar)
{
    if (m_hasScalar == hasScalar)
        return;

    m_hasScalar = hasScalar;
    emit hasScalarChanged(hasScalar);
}

void CPData::setHasVector(bool hasVector)
{
    if (m_hasVector == hasVector)
        return;

    m_hasVector = hasVector;
    emit hasVectorChanged(hasVector);
}

void CPData::setHasArray(bool hasArray)
{
    if (m_hasArray == hasArray)
        return;

    m_hasArray = hasArray;
    emit hasArrayChanged(hasArray);
}

void CPData::setScalarValue(QXYSeries *scalarValue)
{
    if (m_scalarValue == scalarValue)
        return;

    m_scalarValue = scalarValue;
    emit scalarValueChanged(scalarValue);
}
