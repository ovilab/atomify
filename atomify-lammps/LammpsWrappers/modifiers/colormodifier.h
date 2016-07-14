#ifndef COLORMODIFIER_H
#define COLORMODIFIER_H

#include "modifier.h"

class ColorModifier : public Modifier
{
    Q_OBJECT
    Q_PROPERTY(float scale READ scale WRITE setScale NOTIFY scaleChanged)
    float m_scale = 0.6;
public:
    ColorModifier();
    virtual void apply(AtomData &atomData) override;
    float scale() const;
public slots:
    void setScale(float scale);
signals:
    void scaleChanged(float scale);
};

#endif // COLORMODIFIER_H
