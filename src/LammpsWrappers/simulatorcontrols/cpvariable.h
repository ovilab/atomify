#ifndef CPVARIABLE_H
#define CPVARIABLE_H
#include <vector>
#include "simulatorcontrol.h"
class CPVariable : public SimulatorControl
{
    Q_OBJECT
    Q_PROPERTY(Data1D* data READ data WRITE setData NOTIFY dataChanged)
    Q_PROPERTY(bool hovered READ hovered WRITE setHovered NOTIFY hoveredChanged)
    Q_PROPERTY(bool isPerAtom READ isPerAtom WRITE setIsPerAtom NOTIFY isPerAtomChanged)
    Q_PROPERTY(double value READ value WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(bool valueHasDecimals READ valueHasDecimals WRITE setValueHasDecimals NOTIFY valueHasDecimalsChanged)
public:
    CPVariable(Qt3DCore::QNode *parent = 0);
    Q_INVOKABLE void clear();
    Q_INVOKABLE void exportToTextFile(QString fileName);
    Q_INVOKABLE void exportToPythonFile(QString fileName);
    Q_INVOKABLE void exportToMatlabFile(QString fileName);
    virtual bool existsInLammps(class LAMMPSController *lammpsController);
    void synchronize(class LAMMPSController *lammpsController);
    class Data1D* data() const;
    double value() const;
    bool valueHasDecimals() const;
    bool isPerAtom() const;
    bool hovered() const;
    const std::vector<double> &atomData() const;
    void updateData1D();

public slots:
    void setData(class Data1D* data);
    void setValue(double value);
    void setValueHasDecimals(bool valueHasDecimals);
    void setIsPerAtom(bool isPerAtom);
    void setHovered(bool hovered);

signals:
    void dataChanged(class Data1D* data);
    void valueChanged(double value);
    void valueHasDecimalsChanged(bool valueHasDecimals);
    void isPerAtomChanged(bool isPerAtom);
    void hoveredChanged(bool hovered);

protected:
    virtual void updateCommand();
    virtual QList<QString> enabledCommands();
    virtual QList<QString> disableCommands();
    virtual QList<QString> resetCommands();
    QString createCommandPrefix();

private:
    class Data1D* m_data;
    std::vector<double> m_atomData;
    double m_value;
    bool m_valueHasDecimals;
    bool m_isPerAtom;
    bool m_hovered;
};

#endif // CPVARIABLE_H
