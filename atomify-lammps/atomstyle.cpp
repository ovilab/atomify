#include "atomstyle.h"

AtomStyle::AtomStyle(QObject *parent) : QObject(parent)
{

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
    m_red = color.red();
    m_green = color.green();
    m_blue = color.blue();
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

int AtomStyleData::red() const
{
    return m_red;
}

int AtomStyleData::green() const
{
    return m_green;
}

int AtomStyleData::blue() const
{
    return m_blue;
}

void AtomStyleData::setScale(double scale)
{
    if (m_scale == scale)
        return;

    m_scale = scale;
    emit scaleChanged(scale);
}

void AtomStyleData::setRed(int red)
{
    if (m_red == red)
        return;

    m_red = red;
    emit redChanged(red);
    m_color.setRed(m_red);
    emit colorChanged(m_color);
}

void AtomStyleData::setGreen(int green)
{
    if (m_green == green)
        return;

    m_green = green;
    emit greenChanged(green);
    m_color.setGreen(m_green);
    emit colorChanged(m_color);
}

void AtomStyleData::setBlue(int blue)
{
    if (m_blue == blue)
        return;

    m_blue = blue;
    emit blueChanged(blue);
    m_color.setBlue(m_blue);
    emit colorChanged(m_color);
}
