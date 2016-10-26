#ifndef CPVARIABLE_H
#define CPVARIABLE_H
#include "simulatorcontrol.h"
class CPVariable : public SimulatorControl
{
    Q_OBJECT
    Q_PROPERTY(Data1D* data READ data WRITE setData NOTIFY dataChanged)
public:
    CPVariable(Qt3DCore::QNode *parent = 0);
    virtual bool existsInLammps(class LAMMPSController *lammpsController);
    void synchronize(class LAMMPSController *lammpsController);
    class Data1D* data() const;

public slots:
    void setData(class Data1D* data);

signals:
    void dataChanged(class Data1D* data);

protected:
    virtual void updateCommand();
    virtual QList<QString> enabledCommands();
    virtual QList<QString> disableCommands();
    virtual QList<QString> resetCommands();
    QString createCommandPrefix();
private:
    class Data1D* m_data;
};

#endif // CPVARIABLE_H
