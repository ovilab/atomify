#ifndef SIMULATORCONTROL_H
#define SIMULATORCONTROL_H

#include <QObject>
class LAMMPSController;

class SimulatorControl : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool dirty READ dirty WRITE setDirty NOTIFY dirtyChanged)
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
protected:
    bool m_dirty = false;
    bool m_enabled = false;

public:
    explicit SimulatorControl(QObject *parent = 0);
    virtual void synchronizeLammps(LAMMPSController *lammpsController) = 0;
    bool dirty() const;
    bool enabled() const;
signals:

    void dirtyChanged(bool dirty);
    void enabledChanged(bool enabled);

public slots:
    void setDirty(bool dirty);
    void setEnabled(bool enabled);
};

#endif // SIMULATORCONTROL_H
