#include "cpscalardata.h"

CPScalarData::CPScalarData(QObject *parent) : QObject(parent)
{

}

double CPScalarData::current() const
{
    return m_current;
}

void CPScalarData::add(double time, double value)
{
    setCurrent(value);
}

void CPScalarData::setCurrent(double current)
{
    if (m_current == current)
        return;

    m_current = current;
    emit currentChanged(current);
}
