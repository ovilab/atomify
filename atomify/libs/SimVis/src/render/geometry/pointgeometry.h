#ifndef POINTGEOMETRY_H
#define POINTGEOMETRY_H

#include <Qt3DRender/qt3drender_global.h>
#include <Qt3DRender/qgeometry.h>
#include <Qt3DRender/qbuffer.h>
#include <Qt3DRender/qattribute.h>

class PointGeometry : public Qt3DRender::QGeometry
{
    Q_OBJECT

public:
    explicit PointGeometry(QNode *parent = 0);

    void updateVertices();
    void updateIndices();
    void init();

public Q_SLOTS:

Q_SIGNALS:

protected:

private:
    Qt3DRender::QAttribute *m_positionAttribute;
    Qt3DRender::QAttribute *m_idAttribute;
    Qt3DRender::QAttribute *m_normalAttribute;
    Qt3DRender::QAttribute *m_texCoordAttribute;
    Qt3DRender::QAttribute *m_indexAttribute;
    Qt3DRender::QBuffer *m_vertexBuffer;
    Qt3DRender::QBuffer *m_indexBuffer;
};

#endif // POINTGEOMETRY_H
