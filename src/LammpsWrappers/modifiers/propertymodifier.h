#ifndef PROPERTYMODIFIER_H
#define PROPERTYMODIFIER_H

#include "modifier.h"
#include <vector>

class PropertyModifier : public Modifier
{
    Q_OBJECT
    Q_PROPERTY(bool active READ active WRITE setActive NOTIFY activeChanged)
    Q_PROPERTY(double max READ max WRITE setMax NOTIFY maxChanged)
    Q_PROPERTY(double min READ min WRITE setMin NOTIFY minChanged)
public:
    PropertyModifier();

    // Modifier interface
public:
    virtual void apply(class AtomData &atomData) override;
    bool active() const;
    double max() const;
    double min() const;

public slots:
    void setActive(bool active);
    void setMax(double max);
    void setMin(double min);

signals:
    void activeChanged(bool active);
    void maxChanged(double max);
    void minChanged(double min);

private:
    class SimulatorControl *m_previousHovered = nullptr;
    void applyColors(AtomData &atomData, const std::vector<double> &values, int groupBit);
    bool m_active = false;
    std::vector<float> m_cleanPoints;
    double m_max = 0;
    double m_min = 0;
};

#endif // PROPERTYMODIFIER_H
