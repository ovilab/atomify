#ifndef COMPUTE_H
#define COMPUTE_H
#include "simulatorcontrol.h"
class CPCompute : public SimulatorControl
{
    Q_OBJECT
    Q_PROPERTY(Data1D* scalarValue READ scalarValue WRITE setScalarValue NOTIFY scalarValueChanged)
    Q_PROPERTY(QVariantMap vectorValues READ vectorValues WRITE setVectorValues NOTIFY vectorValuesChanged)
    Q_PROPERTY(QStringList vectorTitles READ vectorTitles WRITE setVectorTitles NOTIFY vectorTitlesChanged)
    Q_PROPERTY(QString scalarTitle READ scalarTitle WRITE setScalarTitle NOTIFY scalarTitleChanged)
    Q_PROPERTY(QList<qreal> values READ values WRITE setValues NOTIFY valuesChanged)
    Q_PROPERTY(float value READ value WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(bool timeDependent READ timeDependent WRITE setTimeDependent NOTIFY timeDependentChanged)
    Q_PROPERTY(bool average READ average WRITE setAverage NOTIFY averageChanged)

protected:
    QString m_scalarTitle;
    bool m_average = false;
    class Data1D *m_scalarValue = nullptr;
    QVariantMap m_vectorValues;
    bool m_timeDependent = true;
    float m_value = 0.0;
    QStringList m_vectorTitles;
    QList<qreal> m_values;
    QMap<QString, class Data1D*> m_vectorValuesRaw;

    virtual void updateCommand() override;
    QList<QString> enabledCommands() override;
    QList<QString> disableCommands() override;
    virtual QList<QString> resetCommands() override;
    class Data1D *ensureExists(QString key, bool enabledByDefault);
public:
    explicit CPCompute(QQuickItem *parent = 0);
    ~CPCompute();
    void update(LAMMPSController *lammpsController) override;
    bool existsInLammps(LAMMPSController *lammpsController) override;
    Q_INVOKABLE void clear() override;
    float value() const;
    QList<qreal> values() const;
    QStringList vectorTitles() const;
    QString scalarTitle() const;
    class Data1D* scalarValue() const;
    QVariantMap vectorValues() const;
    bool timeDependent() const;
    bool average() const;

signals:
    void valueChanged(float value);
    void valuesChanged(QList<qreal> values);
    void vectorTitlesChanged(QStringList vectorTitles);
    void scalarTitleChanged(QString scalarTitle);
    void scalarValueChanged(class Data1D *scalarValue);
    void vectorValuesChanged(QVariantMap vectorValues);
    void timeDependentChanged(bool timeDependent);
    void averageChanged(bool average);

public slots:
    void setValue(float value);
    void setValues(QList<qreal> values);
    void setVectorTitles(QStringList vectorTitles);
    void setScalarTitle(QString scalarTitle);
    void setScalarValue(class Data1D *scalarValue);
    void setVectorValues(QVariantMap vectorValues);
    void setTimeDependent(bool timeDependent);
    void setAverage(bool average);
};

#endif // COMPUTE_H
