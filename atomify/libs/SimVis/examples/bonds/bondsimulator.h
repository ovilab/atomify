#ifndef MYSIMULATOR_H
#define MYSIMULATOR_H
#include <SimVis/Simulator>
#include <QVector>
#include <QVector3D>
#include <QElapsedTimer>
#include <Qt3DRender/QBuffer>
#include <SimVis/SphereData>
#include <SimVis/BondData>

class BondWorker : public SimulatorWorker
{
    Q_OBJECT
public:
    BondWorker();

private:
    // SimulatorWorker interface
    virtual void synchronizeSimulator(Simulator *simulator);
    virtual void work();
    QVector<SphereVBOData> m_spheres;
    QVector<BondVBOData> m_bonds;
    float dt = 0.05;
    QElapsedTimer m_timer;
};

class BondSimulator : public Simulator
{
    Q_OBJECT
    Q_PROPERTY(double dt READ dt WRITE setDt NOTIFY dtChanged)
    Q_PROPERTY(SphereData* sphereData READ sphereData CONSTANT)
    Q_PROPERTY(BondData* bondData READ bondData CONSTANT)
    Q_PROPERTY(float radius1 READ radius1 WRITE setRadius1 NOTIFY radius1Changed)
    Q_PROPERTY(float radius2 READ radius2 WRITE setRadius2 NOTIFY radius2Changed)

public:
    BondSimulator(QNode *parent = 0);

    double dt() const;
    SphereData* sphereData();
    BondData* bondData();
    float radius1() const;
    float radius2() const;

public slots:
    void setDt(double dt);
    void setRadius1(float radius1);
    void setRadius2(float radius2);

signals:
    void dtChanged(double dt);
    void radius1Changed(float radius1);
    void radius2Changed(float radius2);

protected:
    virtual SimulatorWorker *createWorker() override;

private:
    double m_dt = 0.05;
    QScopedPointer<SphereData> m_sphereData;
    QScopedPointer<BondData> m_bondData;
    float m_radius2 = 1.0;
    float m_radius1 = 1.0;
};

#endif // MYSIMULATOR_H
