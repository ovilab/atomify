#ifndef FIXAVERAGETIME_H
#define FIXAVERAGETIME_H
#include "cpcompute.h"

class FixAverageTime : public CPCompute
{
    Q_OBJECT
    Q_PROPERTY(int nEvery READ nEvery WRITE setNEvery NOTIFY nEveryChanged)
    Q_PROPERTY(int nRepeat READ nRepeat WRITE setNRepeat NOTIFY nRepeatChanged)
    Q_PROPERTY(int nFreq READ nFreq WRITE setNFreq NOTIFY nFreqChanged)
    Q_PROPERTY(CPCompute* compute READ compute WRITE setCompute NOTIFY computeChanged)
    CPCompute* m_compute = nullptr;

protected:
    int m_nEvery = 1;
    int m_nRepeat = 10;
    int m_nFreq = 1;

public:
    FixAverageTime(Qt3DCore::QNode *parent = 0);

    // SimulatorControl interface
protected:
    virtual void updateCommand() override;
    virtual QList<QString> enabledCommands() override;
    virtual QList<QString> disableCommands() override;
    virtual QList<QString> resetCommands() override;

public:
    CPCompute* compute() const;
    int nEvery() const;
    int nRepeat() const;
    int nFreq() const;
    virtual void update(LAMMPSController *lammpsController) override;
    virtual bool existsInLammps(LAMMPSController *lammpsController) override;

public slots:
    void setNEvery(int NEvery);
    void setNRepeat(int NRepeat);
    void setNFreq(int NFreq);
    void setCompute(CPCompute* compute);

signals:
    void nEveryChanged(int NEvery);
    void nRepeatChanged(int NRepeat);
    void nFreqChanged(int NFreq);
    void computeChanged(CPCompute* compute);
private:
    QT3D_CLONEABLE(FixAverageTime)
};

#endif // FIXAVERAGETIME_H
