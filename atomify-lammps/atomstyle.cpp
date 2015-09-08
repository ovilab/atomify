#include "atomstyle.h"

AtomStyle::AtomStyle(QObject *parent) : QObject(parent)
{
    m_data.push_back(new AtomStyleData(1.0, QColor("red")));
    setModel(QVariant::fromValue(m_data));
}

AtomStyle::~AtomStyle()
{
    m_data.clear();
}

QVariant AtomStyle::model() const
{
    return m_model;
}

void AtomStyle::setColorsAndScales(QVector<QColor> &colors, QVector<float> &scales, QVector<int> &atomTypes)
{
    int numberOfAtoms = colors.size();
    QColor defaultColor = QColor(255.0, 0.0, 0.0);
    double defaultScale = 1.0;
    int numberOfAtomsInStyle = m_data.size();

    for(unsigned int i=0; i<numberOfAtoms; i++) {
        int atomType = atomTypes[i];

        if(atomType > numberOfAtomsInStyle) {
            colors[i] = defaultColor;
            scales[i] = defaultScale;
        } else {
            AtomStyleData *atomStyleData = qobject_cast<AtomStyleData*>(m_data[atomType-1]); // LAMMPS atom types start at 1
            colors[i] = atomStyleData->color();
            scales[i] = atomStyleData->scale();
        }
    }
}

void AtomStyle::setData(QList<QObject *> data)
{
    m_data = data;
}

QList<QObject *> AtomStyle::data()
{
    return m_data;
}

void AtomStyle::setModelData(const int index, const QString &key, const QVariant &value)
{
    QList<QObject*> content = m_model.value<QList<QObject*> >();
    AtomStyleData *obj = qobject_cast<AtomStyleData*>(content[index]);
    obj->setProperty(key.toStdString().c_str(), value);
}

void AtomStyle::setModel(QVariant model)
{
    if (m_model == model)
        return;
    m_model = model;
    emit modelChanged(model);
}

void AtomStyle::add()
{
    double red = 255*(rand()/double(RAND_MAX));
    double green = 255*(rand()/double(RAND_MAX));
    double blue = 255*(rand()/double(RAND_MAX));
    m_data.push_back(new AtomStyleData(1.0, QColor(red,green,blue)));
    setModel(QVariant::fromValue(m_data));
}

AtomStyleData::AtomStyleData(double scale, QColor color)
{
    m_scale = scale;
    m_color = color;
}

double AtomStyleData::scale() const
{
    return m_scale;
}

QColor AtomStyleData::color() const
{
    return m_color;
}

void AtomStyleData::setScale(double scale)
{
    if (m_scale == scale)
        return;

    m_scale = scale;
    emit scaleChanged(scale);
}

void AtomStyleData::setColor(QColor color)
{
    if (m_color == color)
        return;

    m_color = color;
    emit colorChanged(color);
}
