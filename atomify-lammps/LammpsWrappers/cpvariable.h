#ifndef CPVARIABLE_H
#define CPVARIABLE_H
#include "simulatorcontrol.h"

class CPVariable : public SimulatorControl
{
    Q_OBJECT
    Q_PROPERTY(int frequency READ frequency WRITE setFrequency NOTIFY frequencyChanged)
    Q_PROPERTY(Data1D* data READ data WRITE setData NOTIFY dataChanged)
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    QString m_title;

public:
    CPVariable();

protected:
    virtual void updateCommand();
    virtual QList<QString> enabledCommands();
    virtual QList<QString> disableCommands();
    virtual QList<QString> resetCommands();

public:
    virtual bool existsInLammps(class LAMMPSController *lammpsController) override;
    virtual void update(LAMMPSController *lammpsController) override;
    int frequency() const;
    class Data1D* data() const;
    QString title() const;

public slots:
    void setFrequency(int frequency);
    void setData(class Data1D* data);
    void setTitle(QString title);

signals:
    void frequencyChanged(int frequency);
    void dataChanged(class Data1D* data);
    void titleChanged(QString title);

protected:
    class Data1D* m_data = nullptr;
    int m_frequency = 10;
    int m_lastUpdated = -1e6;
};

#endif // CPVARIABLE_H
