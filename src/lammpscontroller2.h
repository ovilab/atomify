#ifndef LAMMPSCONTROLLER2_H
#define LAMMPSCONTROLLER2_H

#include <QBuffer>
#include <QComponent>

namespace atomify {

class LAMMPSController2 : public Qt3DCore::QComponent
{
    Q_OBJECT
    Q_PROPERTY(Qt3DRender::QBuffer* spheresBuffer READ spheresBuffer CONSTANT)

public:
    explicit LAMMPSController2(Qt3DCore::QNode *parent = nullptr);
    Qt3DRender::QBuffer* spheresBuffer() const;

signals:

public slots:

private:
    QScopedPointer<Qt3DRender::QBuffer> m_spheresBuffer;
    Qt3DCore::QNodeCreatedChangeBasePtr createNodeCreationChange() const override;
};

struct LAMMPSControllerData {
    Qt3DCore::QNodeId spheresBufferId;
};

}

#endif // LAMMPSCONTROLLER2_H
