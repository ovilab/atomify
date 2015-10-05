#include "atomstyle.h"

void AtomStyleData::fillInAtomStyleTypes()
{
    m_atomStyleTypes.insert("hydrogen", QPair<float, QString>(1.20, "#FFFFFF"));
    m_atomStyleTypes.insert("helium", QPair<float, QString>(1.40, "#D9FFFF"));
    m_atomStyleTypes.insert("lithium", QPair<float, QString>(1.82, "#CC80FF"));
    m_atomStyleTypes.insert("beryllium", QPair<float, QString>(1.53, "#C2FF00"));
    m_atomStyleTypes.insert("boron", QPair<float, QString>(1.92, "#FFB5B5"));
    m_atomStyleTypes.insert("carbon", QPair<float, QString>(1.70, "#909090"));
    m_atomStyleTypes.insert("nitrogen", QPair<float, QString>(1.55, "#3050F8"));
    m_atomStyleTypes.insert("oxygen", QPair<float, QString>(1.52, "#FF0D0D"));
    m_atomStyleTypes.insert("fluorine", QPair<float, QString>(1.35, "#90E050"));
    m_atomStyleTypes.insert("neon", QPair<float, QString>(1.54, "#3050F8"));
    m_atomStyleTypes.insert("sodium", QPair<float, QString>(2.27, "#AB5CF2"));
    m_atomStyleTypes.insert("magnesium", QPair<float, QString>(1.73, "#8AFF00"));
    m_atomStyleTypes.insert("aluminium", QPair<float, QString>(1.84, "#BFA6A6"));
    m_atomStyleTypes.insert("silicon", QPair<float, QString>(2.27, "#F0C8A0"));
    m_atomStyleTypes.insert("phosphorus", QPair<float, QString>(1.80, "#FF8000"));
    m_atomStyleTypes.insert("sulfur", QPair<float, QString>(1.80, "#FFFF30"));
    m_atomStyleTypes.insert("chlorine", QPair<float, QString>(1.75, "#1FF01F"));
    m_atomStyleTypes.insert("argon", QPair<float, QString>(1.88, "#80D1E3"));
    m_atomStyleTypes.insert("potassium", QPair<float, QString>(2.75, "#8F40D4"));
    m_atomStyleTypes.insert("calcium", QPair<float, QString>(2.31, "#3DFF00"));
}

void AtomStyleData::setType(QString atomTypeName)
{
    atomTypeName = atomTypeName.toLower().trimmed();
    if(m_atomStyleTypes.contains(atomTypeName)) {
        float scale = m_atomStyleTypes[atomTypeName].first;
        QString color = m_atomStyleTypes[atomTypeName].second;
        m_scale = scale;
        m_color = color;
    }
}

AtomStyleData::AtomStyleData(QString atomTypeName) {
    fillInAtomStyleTypes();
    atomTypeName = atomTypeName.toLower().trimmed();
    if(m_atomStyleTypes.contains(atomTypeName)) {
        float scale = m_atomStyleTypes[atomTypeName].first;
        QString color = m_atomStyleTypes[atomTypeName].second;
        m_scale = scale;
        m_color = color;
    } else {
        m_scale = 1.0;
        m_color = "#FFFFFF";
    }
}

AtomStyleData::AtomStyleData(double scale, QColor color)
{
    fillInAtomStyleTypes();
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

bool AtomStyleData::visible() const
{
    return m_visible;
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

void AtomStyleData::setVisible(bool visible)
{
    if (m_visible == visible)
        return;

    m_visible = visible;
    emit visibleChanged(visible);
}

AtomStyleData *AtomStyle::getAtomStyleData(int index)
{
    if(index<0 || index >= m_data.size()) return NULL;

    return qobject_cast<AtomStyleData*>(m_data[index]);
}

AtomStyle::AtomStyle(QObject *parent) : QObject(parent)
{
    m_data.push_back(new AtomStyleData(1.0, QColor(220, 85, 176)));
    m_data.push_back(new AtomStyleData(1.0, QColor(55, 199, 166)));
    m_data.push_back(new AtomStyleData(1.0, QColor(220, 0, 200)));
    setModel(QVariant::fromValue(m_data));
}

AtomStyle::~AtomStyle()
{

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

void AtomStyle::setScaleAndColorForAtom(float scale, QString color, int atomType)
{
    atomType -= 1; // LAMMPS atom types start at 1
    if(atomType<0 || atomType >= m_data.size()) return;

    AtomStyleData *atomStyleData = qobject_cast<AtomStyleData*>(m_data[atomType]);

    if(atomStyleData) {
        atomStyleData->setColor(color);
        atomStyleData->setScale(scale);
    }
    setDirty(true);
}

void AtomStyle::setData(QList<QObject *> data)
{
    m_data = data;
}

QList<QObject *> AtomStyle::data()
{
    return m_data;
}

bool AtomStyle::dirty() const
{
    return m_dirty;
}

void AtomStyle::setModelData(const int index, const QString &key, const QVariant &value)
{
    QList<QObject*> content = m_model.value<QList<QObject*> >();
    AtomStyleData *obj = qobject_cast<AtomStyleData*>(content[index]);
    obj->setProperty(key.toStdString().c_str(), value);
    setDirty(true);
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
    setDirty(true);
}

void AtomStyle::remove(const int index)
{
    m_data.removeAt(index);
    setModel(QVariant::fromValue(m_data));
    setDirty(true);
}

void AtomStyle::setMinimumSize(int minimumSize)
{
    while(m_data.size() < minimumSize) {
        add();
    }
}

void AtomStyle::setDirty(bool dirty)
{
    if (m_dirty == dirty)
        return;

    m_dirty = dirty;
    emit dirtyChanged(dirty);
}

void AtomStyle::setAtomType(QString atomTypeName, int atomType)
{
    AtomStyleData *data = getAtomStyleData(atomType-1); // LAMMPS indices start on 1
    if(data) {
        data->setType(atomTypeName);
        setModel(QVariant::fromValue(m_data));
        setDirty(true);
    }
}

void AtomStyle::updateModel()
{
    setModel(QVariant::fromValue(m_data));
    setDirty(true);
}
