#ifndef QT3DRENDER_CullFaceNone_H
#define QT3DRENDER_CullFaceNone_H

#include <Qt3DRender/qrenderstate.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class CullFaceNonePrivate;

class QT3DRENDERSHARED_EXPORT CullFaceNone : public QRenderState
{
    Q_OBJECT
    Q_PROPERTY(CullingMode mode READ mode WRITE setMode NOTIFY modeChanged)

public:

    enum CullingMode
    {
        Front = 0x0404,
        Back = 0x0405,
        FrontAndBack = 0x0408
    };
    Q_ENUM(CullingMode)

    explicit CullFaceNone(Qt3DCore::QNode *parent = Q_NULLPTR);
    ~CullFaceNone();

    CullingMode mode() const;

public Q_SLOTS:
    void setMode(CullingMode mode);

Q_SIGNALS:
    void modeChanged(CullingMode mode);

protected:
    void copy(const Qt3DCore::QNode *ref) Q_DECL_OVERRIDE;

private:
    Q_DECLARE_PRIVATE(CullFaceNone)
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_CullFaceNone_H
