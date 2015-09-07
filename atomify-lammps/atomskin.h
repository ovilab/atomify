#ifndef ATOMSKIN_H
#define ATOMSKIN_H
#include <QDebug>
#include <QObject>
#include <QColor>

class AtomSkin : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantList colors READ colors WRITE setColors)
    Q_PROPERTY(QList<qreal> scales READ scales WRITE setScales)

    QVector<double> m_scalesVector;
    QVector<QColor> m_colorsVector;
    QVariantList m_colors;
    QList<qreal> m_scales;

public:
    explicit AtomSkin(QObject *parent = 0);
    ~AtomSkin();
    QVariantList colors() const
    {
        return m_colors;
    }

    QList<qreal> scales() const
    {
        return m_scales;
    }

    void setColorsAndScales(QVector<QColor> &colors, QVector<float> &scales, QVector<int> &atomTypes);

signals:

public slots:
    void setColors(QVariantList colors)
    {
        m_colors = colors;
        m_colorsVector.clear();
        for(QVariant &color : colors) {
            m_colorsVector.push_back(color.value<QColor>());
        }
    }

    void setScales(QList<qreal> scales)
    {
        m_scales = scales;
        m_scalesVector = QVector<double>::fromList(scales);
    }
};

#endif // ATOMSKIN_H
