#ifndef CPFIXINDENT_H
#define CPFIXINDENT_H

#include "cpfix.h"

class CPFixIndent : public CPFix
{
    Q_OBJECT
public:
    explicit CPFixIndent(Qt3DCore::QNode *parent = 0);
    ~CPFixIndent();
    enum IdentType
    {
        NONE,
        SPHERE,
        CYLINDER,
        PLANE
    };
    Q_ENUMS(IdentType)

private:

    // CPFix interface
public:
    virtual void copyData(class LAMMPSController *lammpsController) override;
};

#endif // CPFIXINDENT_H
