#ifndef SIMULATORCONTROL_H
#define SIMULATORCONTROL_H
#include <QObject>
#include <QUrl>
#include <QVariantList>
#include "dataproviders/data1d.h"
#include "../../windowgl2.h"

class LAMMPSController;
class AtomifySimulator;
class SimulatorControl : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString type READ type WRITE setType NOTIFY typeChanged)
    Q_PROPERTY(bool hasScalarData READ hasScalarData WRITE setHasScalarData NOTIFY hasScalarDataChanged)
    Q_PROPERTY(bool isPerAtom READ isPerAtom WRITE setIsPerAtom NOTIFY isPerAtomChanged)
    Q_PROPERTY(bool interactive READ interactive WRITE setInteractive NOTIFY interactiveChanged)
    Q_PROPERTY(bool hovered READ hovered WRITE setHovered NOTIFY hoveredChanged)
    Q_PROPERTY(int perAtomIndex READ perAtomIndex WRITE setPerAtomIndex NOTIFY perAtomIndexChanged)
    Q_PROPERTY(int numPerAtomValues READ numPerAtomValues WRITE setNumPerAtomValues NOTIFY numPerAtomValuesChanged)
    Q_PROPERTY(int frequency READ frequency WRITE setFrequency NOTIFY frequencyChanged)
    Q_PROPERTY(float scalarValue READ scalarValue WRITE setScalarValue NOTIFY scalarValueChanged)
    Q_PROPERTY(QString identifier READ identifier WRITE setIdentifier NOTIFY identifierChanged)
    Q_PROPERTY(QVariantMap data1D READ data1D WRITE setData1D NOTIFY data1DChanged)
    Q_PROPERTY(QString group READ group WRITE setGroup NOTIFY groupChanged)
    Q_PROPERTY(QString xLabel READ xLabel WRITE setXLabel NOTIFY xLabelChanged)
    Q_PROPERTY(QString yLabel READ yLabel WRITE setYLabel NOTIFY yLabelChanged)
    Q_PROPERTY(QUrl qmlFileName READ qmlFileName WRITE setQmlFileName NOTIFY qmlFileNameChanged)
    Q_PROPERTY(QQuickWindow* window READ window WRITE setWindow NOTIFY windowChanged)

protected:
    QString m_type;
    QUrl m_qmlFileName;
    bool m_hasScalarData = false;
    bool m_isMirror = false;
    bool m_isPerAtom = false;
    bool m_interactive = false;
    bool m_hovered = false;
    long m_lastUpdate = -1;
    int m_perAtomIndex = 0;
    int m_numPerAtomValues = 0;
    int m_frequency = 1;
    int m_groupBit = 0;
    float m_scalarValue = 10;
    std::vector<double> m_atomData;
    QString m_group;
    QString m_xLabel;
    QString m_yLabel;
    QString m_command;
    QString m_identifier;
    QVariantMap m_data1D;
    QMap<QString, class Data1D*> m_data1DRaw;
    QQuickWindow* m_window = nullptr;
    class Data1D *ensureExists(QString key, bool enabledByDefault);

public:
    explicit SimulatorControl(QObject *parent = 0);
    ~SimulatorControl();
    static int getNextId();
    Q_INVOKABLE void exportToMatlabFile(QString fileName);
    Q_INVOKABLE void exportToPythonFile(QString fileName);
    Q_INVOKABLE void exportToTextFile(QString fileName);
    Q_INVOKABLE void clear();
    QString identifier() const;
    virtual bool existsInLammps(LAMMPSController *lammpsController) = 0;
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
    QString type() const;
    QUrl qmlFileName() const;
    QQuickWindow* window() const;

signals:
    void willBeDestroyed();
    void identifierChanged(QString identifier);
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
    void typeChanged(QString type);
    void qmlFileNameChanged(QUrl qmlFileName);
    void data1DChanged(QVariantMap data1D);
    void windowChanged(QQuickWindow* window);

public slots:
    void setIdentifier(QString identifier);
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
    void setType(QString type);
    void setQmlFileName(QUrl qmlFileName);
    void setWindow(QQuickWindow* window);
};

#endif // SIMULATORCONTROL_H
