#ifndef CYLINDERSIMULATOR_H
#define CYLINDERSIMULATOR_H

#include <SimVis/Simulator>
#include <SimVis/CylinderData>

#include <QVector>
#include <QVector3D>

class CylinderWorker : public SimulatorWorker
{
    Q_OBJECT
private:
    // SimulatorWorker interface
    virtual void synchronizeSimulator(Simulator *simulator);
    virtual void work();
    QVector<CylinderVBOData> m_cylinders;
public:
    CylinderWorker();
};

class CylinderSimulator : public Simulator
{
    Q_OBJECT
    Q_PROPERTY(CylinderData* cylinderData READ cylinderData CONSTANT)
public:
    CylinderSimulator(QNode *parent = nullptr);

    CylinderData *cylinderData() {
        return m_cylinderData.data();
    }

protected:
    virtual SimulatorWorker *createWorker() override;

private:
    QScopedPointer<CylinderData> m_cylinderData;
};

#endif // CYLINDERSIMULATOR_H
