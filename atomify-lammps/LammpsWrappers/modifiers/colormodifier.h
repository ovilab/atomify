#ifndef COLORMODIFIER_H
#define COLORMODIFIER_H

#include "modifier.h"

class ColorModifier : public Modifier
{
    Q_OBJECT
    Q_PROPERTY(float scale READ scale WRITE setScale NOTIFY scaleChanged)
    float m_scale = 0.3;
public:
    ColorModifier(Atoms *atoms, System *system);
    virtual void apply(AtomData &atomData) override;
    float scale() const;
public slots:
    void setScale(float scale);
signals:
    void scaleChanged(float scale);
};

#endif // COLORMODIFIER_H
