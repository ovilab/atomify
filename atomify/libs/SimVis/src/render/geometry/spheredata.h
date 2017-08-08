#ifndef SPHEREDATA_H
#define SPHEREDATA_H

#include <QObject>
#include <Qt3DRender/QBuffer>
#include <Qt3DCore/QNode>
#include <QVector3D>
struct SphereVBOData
{
    QVector3D position;
    QVector3D color;
    float radius;
};

class SphereData : public Qt3DCore::QNode
{
    Q_OBJECT
    Q_PROPERTY(Qt3DRender::QBuffer* buffer READ buffer CONSTANT)
    Q_PROPERTY(int count READ count NOTIFY countChanged)
public:
    explicit SphereData(QNode *parent = 0);

    Qt3DRender::QBuffer* buffer();
    void setData(QVector<SphereVBOData> data);
    void setData(const QVector<QVector3D> &positions, const QVector<QVector3D> &colors, const QVector<float> &scales);
    void setData(QByteArray byteArray, int count);
    void setPositions(QVector<QVector3D> positions, QVector3D color = QVector3D(1.0, 1.0, 1.0), float scale = 1.0);
    int count() const;
signals:

    void countChanged(int count);

public slots:

private:
    QScopedPointer<Qt3DRender::QBuffer> m_buffer;
    int m_count = 0;
};

#endif // SPHEREDATA_H
