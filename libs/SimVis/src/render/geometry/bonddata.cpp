#include "bonddata.h"

#include <QVector3D>

using Qt3DRender::QBuffer;

BondData::BondData(QNode *parent)
    : QNode(parent)
    , m_buffer(new QBuffer(QBuffer::VertexBuffer, this))
{
}

Qt3DRender::QBuffer *BondData::buffer()
{
    return m_buffer.data();
}

void BondData::setData(QVector<BondVBOData> data)
{
    QByteArray ba;
    ba.resize(data.size() * sizeof(BondVBOData));
    BondVBOData *posData = reinterpret_cast<BondVBOData*>(ba.data());
    // TODO can we just set the address here? Instead of copying.
    for(const BondVBOData &pos : data) {
        *posData++ = pos;
    }
    m_buffer->setData(ba);
    m_count = data.count();
    emit countChanged(m_count);
}

int BondData::count() const
{
    return m_count;
}
