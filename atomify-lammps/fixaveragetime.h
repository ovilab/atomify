#ifndef FIXAVERAGETIME_H
#define FIXAVERAGETIME_H
#include "simulatorcontrol.h"

class FixAverageTime : public SimulatorControl
{
    Q_OBJECT
    Q_PROPERTY(int NEvery READ NEvery WRITE setNEvery NOTIFY NEveryChanged)
    Q_PROPERTY(int NRepeat READ NRepeat WRITE setNRepeat NOTIFY NRepeatChanged)
    Q_PROPERTY(int NFreq READ NFreq WRITE setNFreq NOTIFY NFreqChanged)
private:
    int m_NEvery = 10;
    int m_NRepeat = 1;
    int m_NFreq = 10;

public:
    FixAverageTime();

    // SimulatorControl interface
protected:
    virtual void updateCommand() override;
    virtual QList<QString> enabledCommands() override;
    virtual QList<QString> disableCommands() override;
    virtual QList<QString> resetCommands() override;

public:
    virtual bool existsInLammps(LAMMPSController *lammpsController);
    int NEvery() const;
    int NRepeat() const;
    int NFreq() const;

public slots:
    void setNEvery(int NEvery);
    void setNRepeat(int NRepeat);
    void setNFreq(int NFreq);

signals:
    void NEveryChanged(int NEvery);
    void NRepeatChanged(int NRepeat);
    void NFreqChanged(int NFreq);
};

#endif // FIXAVERAGETIME_H
