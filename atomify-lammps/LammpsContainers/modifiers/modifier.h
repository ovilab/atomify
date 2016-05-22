#ifndef MODIFIER_H
#define MODIFIER_H
#include <QObject>
#include <SimVis/SphereData>
#include <SimVis/CylinderData>
#include "../atoms.h"
class Modifier : public QObject
{
    Q_OBJECT
protected:
    class Atoms *m_atoms;
    class System *m_system;
public:
    Modifier(Atoms *atoms, System *system);
    virtual void apply(AtomData &atomData) = 0;
};

#endif // MODIFIER_H
