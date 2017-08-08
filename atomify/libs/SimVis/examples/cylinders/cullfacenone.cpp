
#include "cullfacenone.h"
#include <private/qnode_p.h>
#include <private/qrenderstate_p.h>
#include <Qt3DCore/qscenepropertychange.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class CullFaceNonePrivate : public QRenderStatePrivate
{
public:
    CullFaceNonePrivate()
        : QRenderStatePrivate(QRenderStatePrivate::CullFace)
        , m_mode(CullFaceNone::Back)
    {
    }

    Q_DECLARE_PUBLIC(CullFaceNone)
    CullFaceNone::CullingMode m_mode;
};

CullFaceNone::CullFaceNone(QNode *parent)
    : QRenderState(*new CullFaceNonePrivate, parent)
{
}

CullFaceNone::~CullFaceNone()
{
    QNode::cleanup();
}

void CullFaceNone::copy(const QNode *ref)
{
    QRenderState::copy(ref);
    const CullFaceNone *refState = static_cast<const CullFaceNone*>(ref);
    d_func()->m_mode = refState->d_func()->m_mode;
}

CullFaceNone::CullingMode CullFaceNone::mode() const
{
    Q_D(const CullFaceNone);
    return d->m_mode;
}

void CullFaceNone::setMode(CullFaceNone::CullingMode mode)
{
    Q_D(CullFaceNone);
    if (d->m_mode != mode) {
        d->m_mode = mode;
        emit modeChanged(mode);
    }
}

} // namespace Qt3DRender

QT_END_NAMESPACE
