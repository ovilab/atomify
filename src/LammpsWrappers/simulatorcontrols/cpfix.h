#ifndef CPFIX_H
#define CPFIX_H
#include "simulatorcontrol.h"
#include "dataproviders/data2d.h"

struct _reax_list; // fix for compilation issues on linux
#include <style_fix.h>

class CPFix : public SimulatorControl
{
    Q_OBJECT
public:
    CPFix(QObject *parent = 0);
    ~CPFix();
    virtual bool existsInLammps(class LAMMPSController *lammpsController);
    virtual void copyData(class LAMMPSController *lammpsController);

signals:

public slots:

private:
    long m_nextValidTimestep = -1;
    bool copyData(LAMMPS_NS::FixAveChunk *fix, class LAMMPSController *lammpsController);
    bool copyData(LAMMPS_NS::FixAveHisto *fix, class LAMMPSController *lammpsController);
    bool copyData(LAMMPS_NS::FixAveTime *fix, class LAMMPSController *lammpsController);
    QString getType(LAMMPSController *lammpsController, int which, QString identifier);
};

#endif // CPFIX_H
