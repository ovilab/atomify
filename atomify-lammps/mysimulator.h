#ifndef MYSIMULATOR_H
#define MYSIMULATOR_H
#include <SimVis/Simulator>
#include <functional>
#include <QMap>
#include <QElapsedTimer>
#include <memory>
#include <cmath>

#include "simulations/simulations.h"
#include "lammpscontroller.h"
#include "lammpsoutput.h"
#include "mpi_stubs/mpi.h"
#include "lammps/lammps.h"
using std::function; using std::unique_ptr;

using namespace LAMMPS_NS;

class MyWorker : public SimulatorWorker
{
    Q_OBJECT
public:
    MyWorker();
private:
    struct SliceProperties {
        bool enabled = false;
        QVector3D normal;
        double distance = 0;
        double width = 0;
    };

    QElapsedTimer m_elapsed;
    QElapsedTimer m_sinceStart;
    LAMMPSController m_lammpsController;

    // SimulatorWorker interface
    virtual void synchronizeSimulator(Simulator *simulator) override;
    virtual void synchronizeRenderer(Renderable *renderableObject) override;
    virtual void work() override;
    SliceProperties slice;

    QVector<int> m_atomTypes;
    Simulation *m_currentSimulation = 0;
    QMap<QString, Simulation*> m_simulations;
    void loadSimulation(QString simulationId);
};

class MySimulator : public Simulator
{
    Q_OBJECT
    Q_PROPERTY(int simulationSpeed READ simulationSpeed WRITE setSimulationSpeed NOTIFY simulationSpeedChanged)
    Q_PROPERTY(double simulationTime READ simulationTime WRITE setSimulationTime NOTIFY simulationTimeChanged)
    Q_PROPERTY(bool paused READ paused WRITE setPaused NOTIFY pausedChanged)
    Q_PROPERTY(bool sliceEnabled READ sliceEnabled WRITE setSliceEnabled NOTIFY sliceEnabledChanged)
    Q_PROPERTY(double sliceDistance READ sliceDistance WRITE setSliceDistance NOTIFY sliceDistanceChanged)
    Q_PROPERTY(QVector3D sliceNormal READ sliceNormal WRITE setSliceNormal NOTIFY sliceNormalChanged)
    Q_PROPERTY(double sliceWidth READ sliceWidth WRITE setSliceWidth NOTIFY sliceWidthChanged)
public:
    MySimulator() { }
    ~MySimulator() { }

    // Simulator interface
    int simulationSpeed() const;
    void addCompute(CPCompute *compute);
    QMap<QString, CPCompute *> computes() const;
    void setComputes(const QMap<QString, CPCompute *> &computes);
    LammpsOutput* lammpsOutput() const;
    bool paused() const;
    double simulationTime() const;

    bool sliceEnabled() const
    {
        return m_sliceEnabled;
    }

    double sliceDistance() const
    {
        return m_sliceDistance;
    }

    QVector3D sliceNormal() const
    {
        return m_sliceNormal;
    }

    double sliceWidth() const
    {
        return m_sliceWidth;
    }

public slots:
    void runScript(QString script);
    void setSimulationSpeed(int arg);
    void setLammpsOutput(LammpsOutput* lammpsOutput);
    void setPaused(bool paused);
    void setSimulationTime(double simulationTime);

    void setSliceEnabled(bool sliceEnabled)
    {
        if (m_sliceEnabled == sliceEnabled)
            return;

        qDebug() << "Slice enabled: " << sliceEnabled;
        m_sliceEnabled = sliceEnabled;
        emit sliceEnabledChanged(sliceEnabled);
    }

    void setSliceDistance(double sliceDistance)
    {
        if (m_sliceDistance == sliceDistance)
            return;

        qDebug() << "Slice distance: " << sliceDistance;

        m_sliceDistance = sliceDistance;
        emit sliceDistanceChanged(sliceDistance);
    }

    void setSliceNormal(QVector3D sliceNormal)
    {
        if (m_sliceNormal == sliceNormal)
            return;
        qDebug() << "Slice normal: " << sliceNormal;
        // m_sliceNormal = sliceNormal.normalized();
        m_sliceNormal = sliceNormal;
        emit sliceNormalChanged(sliceNormal);
    }

    void setSliceWidth(double sliceWidth)
    {
        if (m_sliceWidth == sliceWidth)
            return;
        qDebug() << "Slice width: " << sliceWidth;
        m_sliceWidth = sliceWidth;
        emit sliceWidthChanged(sliceWidth);
    }

signals:
    void simulationSpeedChanged(int arg);
    void lammpsOutputChanged(LammpsOutput* lammpsOutput);
    void pausedChanged(bool paused);
    void simulationTimeChanged(double simulationTime);
    void sliceEnabledChanged(bool sliceEnabled);
    void sliceDistanceChanged(double sliceDistance);
    void sliceNormalChanged(QVector3D sliceNormal);
    void sliceWidthChanged(double sliceWidth);

protected:
    virtual MyWorker *createWorker() override;

private:
    friend class MyWorker;
    int m_simulationSpeed = 1;
    bool m_paused = false;
    bool m_willLoadSimulation = false;
    QString m_scriptToRun;
    QMap<QString, CPCompute*> m_computes;
    LammpsOutput *m_lammpsOutput = NULL;
    double m_simulationTime = 0.0;
    bool m_sliceEnabled = false;
    QVector3D m_sliceNormal = QVector3D(1,0,0);
    double m_sliceDistance = 0;
    double m_sliceWidth = 10;
};

#endif // MYSIMULATOR_H
