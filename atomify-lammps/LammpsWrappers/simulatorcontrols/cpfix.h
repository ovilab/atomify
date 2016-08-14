#ifndef CPFIX_H
#define CPFIX_H

#include "simulatorcontrol.h"
class LAMMPSController;
class CPFix : public SimulatorControl
{
    Q_OBJECT
    Q_PROPERTY(QString group READ group WRITE setGroup NOTIFY groupChanged)
    Q_PROPERTY(int frequency READ frequency WRITE setFrequency NOTIFY frequencyChanged)
    Q_PROPERTY(bool interactive READ interactive WRITE setInteractive NOTIFY interactiveChanged)
protected:
    QList<QString> enabledCommands() override;
    QList<QString> disableCommands() override;
    virtual void updateCommand() override;
    virtual QList<QString> resetCommands() override;
    QString createCommandPrefix() override;
    long lastUpdate = -1;
public:
    CPFix(Qt3DCore::QNode *parent = 0);
    ~CPFix();
    void update(LAMMPSController *lammpsController) override;
    virtual bool existsInLammps(LAMMPSController *lammpsController);
    QString group() const;
    int frequency() const;
    void copyData(LAMMPSController *lammpsController);
    bool interactive() const;

signals:
    void groupChanged(QString group);
    void frequencyChanged(int frequency);
    void interactiveChanged(bool interactive);

public slots:
    void setGroup(QString group);
    void setFrequency(int frequency);
    void setInteractive(bool interactive);

private:
    QString m_group = "all";
    int m_frequency = 10;
    bool m_interactive = false;
};

#endif // CPFIX_H
