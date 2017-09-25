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
    Q_PROPERTY(bool hasScalarData READ hasScalarData WRITE setHasScalarData NOTIFY hasScalarDataChanged)
    Q_PROPERTY(float scalarValue READ scalarValue WRITE setScalarValue NOTIFY scalarValueChanged)
    Q_PROPERTY(QString group READ group WRITE setGroup NOTIFY groupChanged)
    Q_PROPERTY(bool isPerAtom READ isPerAtom WRITE setIsPerAtom NOTIFY isPerAtomChanged)
    Q_PROPERTY(int perAtomIndex READ perAtomIndex WRITE setPerAtomIndex NOTIFY perAtomIndexChanged)
    Q_PROPERTY(int numPerAtomValues READ numPerAtomValues WRITE setNumPerAtomValues NOTIFY numPerAtomValuesChanged)
    Q_PROPERTY(int frequency READ frequency WRITE setFrequency NOTIFY frequencyChanged)
    Q_PROPERTY(QString xLabel READ xLabel WRITE setXLabel NOTIFY xLabelChanged)
    Q_PROPERTY(QString yLabel READ yLabel WRITE setYLabel NOTIFY yLabelChanged)
    Q_PROPERTY(bool interactive READ interactive WRITE setInteractive NOTIFY interactiveChanged)
    Q_PROPERTY(bool hovered READ hovered WRITE setHovered NOTIFY hoveredChanged)

protected:
    long m_lastUpdate = -1;
    std::vector<double> m_atomData;
    float m_scalarValue = 10;
    bool m_hasScalarData = false;
    bool m_isMirror = false;
    bool m_isPerAtom = false;
    bool m_interactive = false;
    bool m_hovered = false;
    int m_perAtomIndex = 0;
    int m_numPerAtomValues = 0;
    int m_frequency = 1;
    int m_groupBit = 0;
    QString m_group;
    QString m_xLabel;
    QString m_yLabel;
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
    bool hasScalarData() const;
    float scalarValue() const;
    QString group() const;
    bool isPerAtom() const;
    int perAtomIndex() const;
    int numPerAtomValues() const;
    int frequency() const;
    QString xLabel() const;
    QString yLabel() const;
    bool interactive() const;
    bool hovered() const;
    int groupBit() const;
    void updateData1D();
    const std::vector<double> &atomData() const;

signals:
    void enabledChanged(bool enabled);
    void identifierChanged(QString identifier);
    void commandChanged(QString command);
    void dependenciesChanged(QVariantList dependencies);
    void isMirrorChanged(bool isMirror);
    void willBeDestroyed();
    void data1DChanged(QVariantMap data1D);
    void hasScalarDataChanged(bool hasScalarData);
    void scalarValueChanged(float scalarValue);
    void groupChanged(QString group);
    void isPerAtomChanged(bool isPerAtom);
    void perAtomIndexChanged(int perAtomIndex);
    void numPerAtomValuesChanged(int numPerAtomValues);
    void frequencyChanged(int frequency);
    void xLabelChanged(QString xLabel);
    void yLabelChanged(QString yLabel);
    void interactiveChanged(bool interactive);
    void hoveredChanged(bool hovered);

public slots:
    void setEnabled(bool enabled);
    void setIdentifier(QString identifier);
    void setCommand(QString command);
    void setDependencies(QVariantList dependencies);
    void setIsMirror(bool isMirror);
    void setData1D(QVariantMap data1D);
    void setHasScalarData(bool hasScalarData);
    void setScalarValue(float scalarValue);
    void setGroup(QString group);
    void setIsPerAtom(bool isPerAtom);
    void setPerAtomIndex(int perAtomIndex);
    void setNumPerAtomValues(int numPerAtomValues);
    void setFrequency(int frequency);
    void setXLabel(QString xLabel);
    void setYLabel(QString yLabel);
    void setInteractive(bool interactive);
    void setHovered(bool hovered);
};

#endif // SIMULATORCONTROL_H
