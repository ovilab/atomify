#ifndef SIMULATORCONTROL_H
#define SIMULATORCONTROL_H
#include <Qt3DCore>
#include <QQuickItem>
#include <QVariantList>
#include "dataproviders/data1d.h"

class LAMMPSController;
class AtomifySimulator;
class SimulatorControl : public Qt3DCore::QNode
{
    Q_OBJECT
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(bool isMirror READ isMirror WRITE setIsMirror NOTIFY isMirrorChanged)
    Q_PROPERTY(QString identifier READ identifier WRITE setIdentifier NOTIFY identifierChanged)
    Q_PROPERTY(QString command READ command WRITE setCommand NOTIFY commandChanged)
    Q_PROPERTY(QVariantList dependencies READ dependencies WRITE setDependencies NOTIFY dependenciesChanged)
    Q_PROPERTY(QVariantMap data1D READ data1D WRITE setData1D NOTIFY data1DChanged)

protected:
    bool m_isMirror = false;
    QString m_command;
    QString m_identifier;
    QString m_identifierPrefix = "atomify_";
    QVariantList m_dependencies;
    QVariantMap m_data1D;
    QMap<QString, class Data1D*> m_data1DRaw;
    bool m_enabled = true;
    bool addToLammps(LAMMPSController *lammpsController);
    virtual void updateCommand() = 0;
    virtual QList<QString> enabledCommands() = 0;
    virtual QList<QString> disableCommands() = 0;
    virtual QList<QString> resetCommands() = 0;
    virtual QString createCommandPrefix() = 0;
    void addDependency(SimulatorControl *control);
    void removeDependency(SimulatorControl *control);
    class Data1D *ensureExists(QString key, bool enabledByDefault);

public:
    explicit SimulatorControl(Qt3DCore::QNode *parent = 0);
    static int getNextId();
    ~SimulatorControl();
    bool enabled() const;
    QString identifier() const;
    QString command() const;
    QVariantList dependencies() const;
    bool dependenciesValid(LAMMPSController *lammpsController);
    virtual bool existsInLammps(LAMMPSController *lammpsController) = 0;
    virtual void update(LAMMPSController *lammpsController);
    virtual void handleCommand(QString command);
    virtual QString fullCommand();
    bool isMirror() const;

    QVariantMap data1D() const;

signals:
    void enabledChanged(bool enabled);
    void identifierChanged(QString identifier);
    void commandChanged(QString command);
    void dependenciesChanged(QVariantList dependencies);
    void isMirrorChanged(bool isMirror);
    void willBeDestroyed();

    void data1DChanged(QVariantMap data1D);

public slots:
    void setEnabled(bool enabled);
    void setIdentifier(QString identifier);
    void setCommand(QString command);
    void setDependencies(QVariantList dependencies);
    void setIsMirror(bool isMirror);
    void setData1D(QVariantMap data1D);
};

#endif // SIMULATORCONTROL_H
