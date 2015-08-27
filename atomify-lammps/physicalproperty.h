#ifndef PHYSICALPROPERTY_H
#define PHYSICALPROPERTY_H

#include <QObject>
#include "lammpscontroller.h"
class MySimulator;

class PhysicalProperty : public QObject
{
    Q_OBJECT
    Q_PROPERTY(MySimulator* simulator READ simulator WRITE setSimulator NOTIFY simulatorChanged)
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(QString command READ command WRITE setCommand NOTIFY commandChanged)
    Q_PROPERTY(QString identifier READ identifier WRITE setIdentifier NOTIFY identifierChanged)
    Q_PROPERTY(double scalar READ scalar NOTIFY valueChanged)
    Q_PROPERTY(QStringList dependencies READ dependencies WRITE setDependencies NOTIFY dependenciesChanged)

public:
    enum Type {Scalar = 0, Vector = 1, Array = 2, PerAtom = 3, Local = 4};

    explicit PhysicalProperty(QObject *parent = 0);
    MySimulator *simulator() const;
    bool enabled() const;
    QString command() const;
    QString identifier() const;
    void addToLAMMPS(LAMMPSController *lammpsController);
    void compute();
    double scalar() const;
    QStringList dependencies() const;
    bool valuesDirty() const;
    void setValuesDirty(bool valuesDirty);

public slots:
    void setSimulator(MySimulator *simulator);
    void setEnabled(bool enabled);
    void setCommand(QString command);
    void setIdentifier(QString identifier);
    void setDependencies(QStringList dependencies);

signals:
    void simulatorChanged(MySimulator *simulator);
    void enabledChanged(bool enabled);
    void commandChanged(QString command);
    void identifierChanged(QString identifier);
    void valueChanged();
    void dependenciesChanged(QStringList dependencies);

private:
    LAMMPSController *m_lammpsController = NULL;
    Compute *m_compute = NULL;
    MySimulator *m_simulator = NULL;
    Type m_type = Scalar;
    bool m_enabled = true;
    bool m_valuesDirty = false;
    QString m_command;
    QString m_identifier;
    QStringList m_dependencies;
};

#endif // PHYSICALPROPERTY_H
