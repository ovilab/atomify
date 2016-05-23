#ifndef MODIFIER_H
#define MODIFIER_H
#include <QObject>
#include <SimVis/SphereData>
#include <SimVis/CylinderData>
#include "../atomdata.h"

class Modifier : public QObject
{
    Q_OBJECT
protected:
    class System *m_system;
public:
    Modifier(class System *system);
    virtual void apply(AtomData &atomData) = 0;
};

#endif // MODIFIER_H
