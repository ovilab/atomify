#ifndef LAMMPSCONTROLLER2_H
#define LAMMPSCONTROLLER2_H

#include <QBuffer>
#include <QComponent>

namespace atomify {

class LAMMPSController2 : public Qt3DCore::QComponent
{
    Q_OBJECT
    Q_PROPERTY(Qt3DRender::QBuffer* spheresBuffer READ spheresBuffer CONSTANT)
    Q_PROPERTY(int visibleAtomCount READ visibleAtomCount NOTIFY visibleAtomCountChanged)

public:
    explicit LAMMPSController2(Qt3DCore::QNode *parent = nullptr);
    Qt3DRender::QBuffer* spheresBuffer() const;
    int visibleAtomCount() const;

signals:
    void visibleAtomCountChanged();

public slots:

private:
    Qt3DCore::QNodeCreatedChangeBasePtr createNodeCreationChange() const override;
    virtual void sceneChangeEvent(const Qt3DCore::QSceneChangePtr &change) override;

    QScopedPointer<Qt3DRender::QBuffer> m_spheresBuffer;
    int m_visibleAtomCount = 0;

};


struct LAMMPSControllerData {
    Qt3DCore::QNodeId spheresBufferId;
};

}

#endif // LAMMPSCONTROLLER2_H
