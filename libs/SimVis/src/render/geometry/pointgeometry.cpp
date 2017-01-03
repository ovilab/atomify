#include "pointgeometry.h"
#include <Qt3DRender/qbuffer.h>
#include <Qt3DRender/QBufferDataGenerator>
#include <Qt3DRender/qattribute.h>
#include <qmath.h>
#include <QVector3D>

using namespace Qt3DRender;

class PointVertexDataFunctor : public Qt3DRender::QBufferDataGenerator
{
public:
    PointVertexDataFunctor()
    {
    }

    QByteArray operator ()() Q_DECL_OVERRIDE
    {
        const int verticesCount = 6;
        // vec3 pos
        const quint32 vertexSize = (3+2+1) * sizeof(float);

        QByteArray verticesData;
        verticesData.resize(vertexSize*verticesCount);
        float *verticesPtr = reinterpret_cast<float*>(verticesData.data());

        *verticesPtr++ = 0.0;
        *verticesPtr++ = 0.0;
        *verticesPtr++ = 0.0;

        *verticesPtr++ = -1.0;
        *verticesPtr++ = -1.0;

        *verticesPtr++ = 0.0;

        *verticesPtr++ = 0.0;
        *verticesPtr++ = 0.0;
        *verticesPtr++ = 0.0;

        *verticesPtr++ = 1.0;
        *verticesPtr++ = -1.0;

        *verticesPtr++ = 1.0;

        *verticesPtr++ = 0.0;
        *verticesPtr++ = 0.0;
        *verticesPtr++ = 0.0;

        *verticesPtr++ = -1.0;
        *verticesPtr++ = 1.0;

        *verticesPtr++ = 2.0;

        *verticesPtr++ = 0.0;
        *verticesPtr++ = 0.0;
        *verticesPtr++ = 0.0;

        *verticesPtr++ = 1.0;
        *verticesPtr++ = 1.0;

        *verticesPtr++ = 3.0;

        *verticesPtr++ = 0.0;
        *verticesPtr++ = 0.0;
        *verticesPtr++ = 0.0;

        *verticesPtr++ = 1.0;
        *verticesPtr++ = 1.0;

        *verticesPtr++ = 4.0;

        *verticesPtr++ = 0.0;
        *verticesPtr++ = 0.0;
        *verticesPtr++ = 0.0;

        *verticesPtr++ = 1.0;
        *verticesPtr++ = 1.0;

        *verticesPtr++ = 5.0;

        return verticesData;
    }

    bool operator ==(const QBufferDataGenerator &other) const Q_DECL_OVERRIDE
    {
        Q_UNUSED(other);
        return true;
    }

    QT3D_FUNCTOR(PointVertexDataFunctor)

private:
};

class PointIndexDataFunctor : public QBufferDataGenerator
{
public:
    PointIndexDataFunctor()
    {
    }

    QByteArray operator ()() Q_DECL_OVERRIDE
    {
        const int indexCount = 6;
        const int indexSize = sizeof(quint16);
        QByteArray indicesBytes;
        indicesBytes.resize(indexCount * indexSize);
        quint16 *indicesPtr = reinterpret_cast<quint16*>(indicesBytes.data());
        *indicesPtr++ = 0;
        *indicesPtr++ = 1;
        *indicesPtr++ = 2;
        *indicesPtr++ = 3;
        *indicesPtr++ = 4;
        *indicesPtr++ = 5;
        return indicesBytes;
    }

    bool operator ==(const QBufferDataGenerator &other) const Q_DECL_OVERRIDE
    {
        Q_UNUSED(other);
        return true;
    }

    QT3D_FUNCTOR(PointIndexDataFunctor)

private:
};

void PointGeometry::init()
{
    m_positionAttribute = new QAttribute(this);
    m_texCoordAttribute = new QAttribute(this);
    m_idAttribute = new QAttribute(this);
    m_indexAttribute = new QAttribute(this);
    m_vertexBuffer = new QBuffer(QBuffer::VertexBuffer, this);
    m_indexBuffer = new QBuffer(QBuffer::IndexBuffer, this);

    const quint32 elementSize = (3+2+1);
    const quint32 stride = elementSize * sizeof(float);
    const int vertexCount = 6;
    const int indexCount = 6;

    m_positionAttribute->setName(QAttribute::defaultPositionAttributeName());
    m_positionAttribute->setDataType(QAttribute::Float);
    m_positionAttribute->setDataSize(3);
    m_positionAttribute->setAttributeType(QAttribute::VertexAttribute);
    m_positionAttribute->setBuffer(m_vertexBuffer);
    m_positionAttribute->setByteStride(stride);
    m_positionAttribute->setCount(vertexCount);

    m_texCoordAttribute->setName(QAttribute::defaultTextureCoordinateAttributeName());
    m_texCoordAttribute->setDataType(QAttribute::Float);
    m_texCoordAttribute->setDataSize(2);
    m_texCoordAttribute->setAttributeType(QAttribute::VertexAttribute);
    m_texCoordAttribute->setBuffer(m_vertexBuffer);
    m_texCoordAttribute->setByteStride(stride);
    m_texCoordAttribute->setByteOffset(3 * sizeof(float));
    m_texCoordAttribute->setCount(vertexCount);

    m_idAttribute->setName("vertexId");
    m_idAttribute->setDataType(QAttribute::Float);
    m_idAttribute->setDataSize(1);
    m_idAttribute->setAttributeType(QAttribute::VertexAttribute);
    m_idAttribute->setBuffer(m_vertexBuffer);
    m_idAttribute->setByteStride(stride);
    m_idAttribute->setByteOffset((3 + 2) * sizeof(float));
    m_idAttribute->setCount(vertexCount);

    m_indexAttribute->setAttributeType(QAttribute::IndexAttribute);
    m_indexAttribute->setDataType(QAttribute::UnsignedShort);
    m_indexAttribute->setBuffer(m_indexBuffer);
    m_indexAttribute->setCount(indexCount);

    m_vertexBuffer->setDataGenerator(QBufferDataGeneratorPtr(new PointVertexDataFunctor()));
    m_indexBuffer->setDataGenerator(QBufferDataGeneratorPtr(new PointIndexDataFunctor()));

    addAttribute(m_positionAttribute);
    addAttribute(m_texCoordAttribute);
    addAttribute(m_idAttribute);
    addAttribute(m_indexAttribute);
}

PointGeometry::PointGeometry(Qt3DCore::QNode *parent)
    : QGeometry(parent)
{
    init();
}

void PointGeometry::updateVertices()
{
}

/*!
 * Updates the indices based on rings and slices.
 */
void PointGeometry::updateIndices()
{
}
