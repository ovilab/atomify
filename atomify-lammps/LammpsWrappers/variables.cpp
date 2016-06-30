#include "variables.h"

Variables::Variables(QObject *parent) : QObject(parent)
{

}

int Variables::count() const
{
    return m_count;
}

QVariant Variables::model() const
{
    return m_model;
}

void Variables::setCount(int count)
{
    if (m_count == count)
        return;

    m_count = count;
    emit countChanged(count);
}

void Variables::setModel(QVariant model)
{
    if (m_model == model)
        return;

    m_model = model;
    emit modelChanged(model);
}
