#ifndef CPVARIABLE_H
#define CPVARIABLE_H
#include "simulatorcontrol.h"
class CPVariable : public SimulatorControl
{
    Q_OBJECT
    Q_PROPERTY(Data1D* data READ data WRITE setData NOTIFY dataChanged)
    Q_PROPERTY(double value READ value WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(bool valueHasDecimals READ valueHasDecimals WRITE setValueHasDecimals NOTIFY valueHasDecimalsChanged)
public:
    CPVariable(Qt3DCore::QNode *parent = 0);
    virtual bool existsInLammps(class LAMMPSController *lammpsController);
    void synchronize(class LAMMPSController *lammpsController);
    class Data1D* data() const;
    double value() const;
    bool valueHasDecimals() const;

public slots:
    void setData(class Data1D* data);
    void setValue(double value);
    void setValueHasDecimals(bool valueHasDecimals);

signals:
    void dataChanged(class Data1D* data);
    void valueChanged(double value);
    void valueHasDecimalsChanged(bool valueHasDecimals);

protected:
    virtual void updateCommand();
    virtual QList<QString> enabledCommands();
    virtual QList<QString> disableCommands();
    virtual QList<QString> resetCommands();
    QString createCommandPrefix();
private:
    class Data1D* m_data;
    double m_value;
    bool m_valueHasDecimals;
};

#endif // CPVARIABLE_H
