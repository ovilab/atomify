#include "uniformgridgeometry.h"
#include <Qt3DRender/QBuffer>
#include <Qt3DRender/QBufferDataGenerator>
#include <Qt3DRender/QAttribute>
#include <qmath.h>
#include <QVector3D>

using namespace Qt3DRender;

class UniformGridVertexDataFunctor : public Qt3DRender::QBufferDataGenerator
{
    int m_vertexCount = 0;
public:
    UniformGridVertexDataFunctor(int vertexCount) { m_vertexCount = vertexCount; }
    void setVertexCount(int vertexCount) { m_vertexCount = vertexCount; }

    QByteArray operator ()() Q_DECL_OVERRIDE
    {
        const quint32 vertexSize = 3*sizeof(float);
        QByteArray verticesData;
        verticesData.resize(vertexSize*m_vertexCount);
        float *verticesPtr = reinterpret_cast<float*>(verticesData.data());
        int verticesPerDimension = cbrt(m_vertexCount); // Since m_vertexCount comes from resolution^3, this will always work
        int N = verticesPerDimension;
        int nHalf = 0.5*N;
        int nHalfSquared = nHalf*nHalf;

        int count = 0;
        for(int i=0; i<N; i++) {
            float x = -nHalf + i;
            for(int j=0; j<N; j++) {
                float y = -nHalf + j;
                for(int k=0; k<N; k++) {
                    float z = -nHalf + k;
                    if(x*x + y*y + z*z < nHalfSquared) {
                        *verticesPtr++ = x;
                        *verticesPtr++ = y;
                        *verticesPtr++ = z;
                        count+=3;
                    }
                }
            }
        }

        verticesData.resize(count*sizeof(float));
        return verticesData;
    }

    bool operator ==(const QBufferDataGenerator &other) const Q_DECL_OVERRIDE
    {
        Q_UNUSED(other);
        return true;
    }

    QT3D_FUNCTOR(UniformGridVertexDataFunctor)
};

void UniformGridGeometry::init()
{
    m_positionAttribute = new QAttribute(this);
    m_vertexBuffer = new QBuffer(QBuffer::VertexBuffer, this);

    const quint32 stride = 3 * sizeof(float); // 3 floats, x y z

    m_positionAttribute->setName(QAttribute::defaultPositionAttributeName());
    m_positionAttribute->setDataType(QAttribute::Float);
    m_positionAttribute->setDataSize(3);
    m_positionAttribute->setAttributeType(QAttribute::VertexAttribute);
    m_positionAttribute->setBuffer(m_vertexBuffer);
    m_positionAttribute->setByteStride(stride);

    m_dataFunctor = new UniformGridVertexDataFunctor(vertexCount());
    QByteArray gridPoints = (*m_dataFunctor)();
    int realVertexCount = gridPoints.size() / (3*sizeof(float));
    m_positionAttribute->setCount(realVertexCount);

    m_vertexBuffer->setDataGenerator(QBufferDataGeneratorPtr(m_dataFunctor));

    addAttribute(m_positionAttribute);
}

UniformGridGeometry::UniformGridGeometry(Qt3DCore::QNode *parent)
    : QGeometry(parent)
{
    init();
}

void UniformGridGeometry::updateVertices()
{
    qWarning() << "UniformGridGeometry::updateVertices() not implemented";
}

/*!
 * Updates the indices based on rings and slices.
 */
void UniformGridGeometry::updateIndices()
{
    qWarning() << "UniformGridGeometry::updateIndices()";
}

int UniformGridGeometry::resolution() const
{
    return m_resolution;
}

void UniformGridGeometry::setResolution(int resolution)
{
    if (m_resolution == resolution)
        return;

    m_resolution = resolution;
    emit resolutionChanged(resolution);
}
