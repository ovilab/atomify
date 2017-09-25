#ifndef CPFIX_H
#define CPFIX_H
#include "simulatorcontrol.h"
struct _reax_list; // fix for compilation issues on linux
#include <style_fix.h>

class CPFix : public SimulatorControl
{
    Q_OBJECT
    Q_PROPERTY(QString group READ group WRITE setGroup NOTIFY groupChanged)
    Q_PROPERTY(int frequency READ frequency WRITE setFrequency NOTIFY frequencyChanged)
    Q_PROPERTY(bool interactive READ interactive WRITE setInteractive NOTIFY interactiveChanged)
    Q_PROPERTY(QVariantList data READ data WRITE setData NOTIFY dataChanged)
    Q_PROPERTY(QVariant model READ model WRITE setModel NOTIFY modelChanged)
    Q_PROPERTY(Data1D* histogram READ histogram WRITE setHistogram NOTIFY histogramChanged)
protected:
    QList<QString> enabledCommands() override;
    QList<QString> disableCommands() override;
    virtual void updateCommand() override;
    virtual QList<QString> resetCommands() override;
    QString createCommandPrefix() override;
    long lastUpdate = -1;

public:
    CPFix(Qt3DCore::QNode *parent = 0);
    ~CPFix();
    void update(class LAMMPSController *lammpsController) override;
    virtual bool existsInLammps(class LAMMPSController *lammpsController);
    QString group() const;
    int frequency() const;
    void copyData(class LAMMPSController *lammpsController);
    bool interactive() const;
    QVariantList data() const;
    QVariant model() const;
    class Data1D* histogram() const;

signals:
    void groupChanged(QString group);
    void frequencyChanged(int frequency);
    void interactiveChanged(bool interactive);
    void dataChanged(QVariantList data);
    void updatedContent();
    void modelChanged(QVariant model);
    void histogramChanged(class Data1D* histogram);

public slots:
    void setGroup(QString group);
    void setFrequency(int frequency);
    void setInteractive(bool interactive);
    void setData(QVariantList data);
    void setModel(QVariant model);
    void setHistogram(class Data1D* histogram);

private:
    QString m_group = "all";
    int m_frequency = 10;
    long m_nextValidTimestep = -1;
    bool m_interactive = false;
    bool copyData(LAMMPS_NS::FixAveChunk *fix, class LAMMPSController *lammpsController);
    bool copyData(LAMMPS_NS::FixAveHisto *fix, class LAMMPSController *lammpsController);
    bool copyData(LAMMPS_NS::FixAveTime *fix, class LAMMPSController *lammpsController);
    QString getType(LAMMPSController *lammpsController, int which, QString identifier);
    QVariantList m_data;
    QList<class Data2D*> m_dataRaw;
    QVariant m_model;
    class Data1D* m_histogram;
};

#endif // CPFIX_H
