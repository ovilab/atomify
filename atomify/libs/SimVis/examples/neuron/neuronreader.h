#ifndef NEURONREADER_H
#define NEURONREADER_H

#include <SimVis/Simulator>
#include <SimVis/CylinderData>
#include <QObject>
#include <QUrl>
#include <QVector3D>

class Segment {
public:
    int id = -1;
    int parentID = -1;
    Segment* parent;
    QVector3D proximal;
    QVector3D distal;
    double proximalWidth = 0.0;
    double distalWidth = 0.0;
    bool hasParentID = false;
    bool hasProximal = false;
    bool hasDistal = false;
};

class NeuronWorker : public SimulatorWorker
{
    Q_OBJECT
public:
    NeuronWorker();

private:
    QVector<Segment> m_segments;
    QVector<QVector3D> m_spheres;
    virtual void synchronizeSimulator(Simulator *simulator) override;
    virtual void work() override;
    void reset();
};

class NeuronReader : public Simulator
{
    Q_OBJECT
    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(CylinderData* cylinderData READ cylinderData CONSTANT)
public:
    explicit NeuronReader(QNode *parent = 0);
    ~NeuronReader();

    QUrl source() const;

    CylinderData *cylinderData() {
        return m_cylinderData.data();
    }

    void readFile();

signals:

    void sourceChanged(QUrl arg);

public slots:

    void setSource(QUrl arg);

    // Simulator interface
protected:
    SimulatorWorker *createWorker() override;

private:
    QUrl m_source;
    QVector<Segment> m_segments;
    QVector<CylinderVBOData> m_cylinders;
    QVector<QVector3D> m_spheres;
    QScopedPointer<CylinderData> m_cylinderData;
    bool m_segmentsAreDirty = true;

    friend class NeuronWorker;
};

#endif // NEURONREADER_H
