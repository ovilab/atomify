#include "cylinderdata.h"

#include <QVector3D>

using Qt3DRender::QBuffer;

CylinderData::CylinderData(QNode *parent)
    : QNode(parent)
    , m_buffer(new QBuffer(QBuffer::VertexBuffer, this))
{
}

Qt3DRender::QBuffer *CylinderData::buffer()
{
    return m_buffer.data();
}

void CylinderData::setData(QVector<CylinderVBOData> data)
{
    QByteArray ba;
    ba.resize(data.size() * sizeof(CylinderVBOData));
    CylinderVBOData *posData = reinterpret_cast<CylinderVBOData*>(ba.data());
    // TODO can we just set the address here? Instead of copying.
    for(const CylinderVBOData &pos : data) {
        *posData++ = pos;
    }
    m_buffer->setData(ba);
    m_count = data.count();
    emit countChanged(m_count);
}

int CylinderData::count() const
{
    return m_count;
}
