#ifndef CPFIX_H
#define CPFIX_H

#include "simulatorcontrol.h"
class LAMMPSController;
class CPFix : public SimulatorControl
{
    Q_OBJECT
    Q_PROPERTY(QString group READ group WRITE setGroup NOTIFY groupChanged)
protected:
    QList<QString> enabledCommands() override;
    QList<QString> disableCommands() override;
    virtual void updateCommand() override;
    virtual QList<QString> resetCommands() override;
    QString createCommandPrefix() override;
public:
    CPFix(Qt3DCore::QNode *parent = 0);
    ~CPFix();
    void update(LAMMPSController *lammpsController) override;
    virtual bool existsInLammps(LAMMPSController *lammpsController);
    QString group() const;

signals:

    void groupChanged(QString group);

public slots:
    void setGroup(QString group);

private:
    QString m_group = "all";
};

#endif // CPFIX_H
