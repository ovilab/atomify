#ifndef CAMERALENS_P_H
#define CAMERALENS_P_H

#include "cameralens.h"

#include <QObject>
#include <QMatrix4x4>

class CameraLensPrivate
{
public:
    CameraLensPrivate(CameraLens *qq);

    inline void updateProjectionMatrix()
    {
        switch (m_projectionType) {
        case CameraLens::OrthogonalProjection:
            updateOrthogonalProjection();
            break;
        case CameraLens::PerspectiveProjection:
            updatePerpectiveProjection();
            break;
        }
    }

    Q_DECLARE_PUBLIC(CameraLens)

    CameraLens::ProjectionType m_projectionType;

    float m_nearPlane;
    float m_farPlane;

    float m_fieldOfView;
    float m_aspectRatio;

    float m_left;
    float m_right;
    float m_bottom;
    float m_top;

    mutable QMatrix4x4 m_projectionMatrix;

    CameraLens *q_ptr;

private:
    inline void updatePerpectiveProjection()
    {
        Q_Q(CameraLens);
        m_projectionMatrix.setToIdentity();
        m_projectionMatrix.perspective(m_fieldOfView, m_aspectRatio, m_nearPlane, m_farPlane);
        Q_EMIT q->projectionMatrixChanged();
    }

    inline void updateOrthogonalProjection()
    {
        Q_Q(CameraLens);
        m_projectionMatrix.setToIdentity();
        m_projectionMatrix.ortho(m_left, m_right, m_bottom, m_top, m_nearPlane, m_farPlane);
        Q_EMIT q->projectionMatrixChanged();
    }
};

#endif // CAMERALENS_P_H

