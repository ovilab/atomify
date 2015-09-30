#include "cameralens.h"
#include "cameralens_p.h"

#include <QMatrix4x4>

CameraLensPrivate::CameraLensPrivate(CameraLens *qq)
//    : QComponentPrivate(qq)
    : q_ptr(qq)
    , m_projectionType(CameraLens::OrthogonalProjection)
    , m_nearPlane(0.1f)
    , m_farPlane(1024.0f)
    , m_fieldOfView(25.0f)
    , m_aspectRatio(1.0f)
    , m_left(-1)
    , m_right(1)
    , m_bottom(-1)
    , m_top(1)
{
}

//CameraLens::CameraLens(QNode *parent)
//    : QComponent(*new CameraLensPrivate(this), parent)
CameraLens::CameraLens(QObject *parent)
    : QObject(parent)
    , d_ptr(new CameraLensPrivate(this))
{
    Q_D(CameraLens);
    d->updateProjectionMatrix();
}

//void CameraLens::copy(const QNode *ref)
//{
//    QComponent::copy(ref);
//    const CameraLens *lens = static_cast<const CameraLens*>(ref);
//    d_func()->m_projectionType = lens->d_func()->m_projectionType;
//    d_func()->m_nearPlane = lens->d_func()->m_nearPlane;
//    d_func()->m_farPlane = lens->d_func()->m_farPlane;
//    d_func()->m_fieldOfView = lens->d_func()->m_fieldOfView;
//    d_func()->m_aspectRatio = lens->d_func()->m_aspectRatio;
//    d_func()->m_left = lens->d_func()->m_left;
//    d_func()->m_right = lens->d_func()->m_right;
//    d_func()->m_bottom = lens->d_func()->m_bottom;
//    d_func()->m_top = lens->d_func()->m_top;
//    d_func()->m_projectionMatrix = lens->d_func()->m_projectionMatrix;
//}

//CameraLens::CameraLens(CameraLensPrivate &dd, QNode *parent)
//    : QComponent(dd, parent)
//{
//    Q_D(CameraLens);
//    d->updateOrthogonalProjection();
//}

void CameraLens::setProjectionType(CameraLens::ProjectionType projectionType)
{
    Q_D(CameraLens);
    if (d->m_projectionType != projectionType) {
        d->m_projectionType = projectionType;
        emit projectionTypeChanged();
        d->updateProjectionMatrix();
    }
}

CameraLens::ProjectionType CameraLens::projectionType() const
{
    Q_D(const CameraLens);
    return d->m_projectionType;
}

void CameraLens::setOrthographicProjection( float left, float right,
                                             float bottom, float top,
                                             float nearPlane, float farPlane )
{
    Q_D(CameraLens);
//    bool block = blockNotifications(true);
    setLeft(left);
    setRight(right);
    setBottom(bottom);
    setTop(top);
    setNearPlane(nearPlane);
    setFarPlane(farPlane);
    setProjectionType(OrthogonalProjection);
//    blockNotifications(block);
    d->updateProjectionMatrix();
}

void CameraLens::setPerspectiveProjection( float fieldOfView, float aspectRatio,
                                            float nearPlane, float farPlane )
{
    Q_D(CameraLens);
//    bool block = blockNotifications(true);
    setFieldOfView(fieldOfView);
    setAspectRatio(aspectRatio);
    setNearPlane(nearPlane);
    setFarPlane(farPlane);
    setProjectionType(PerspectiveProjection);
//    blockNotifications(block);
    d->updateProjectionMatrix();
}

void CameraLens::setNearPlane(float nearPlane)
{
    Q_D(CameraLens);
    if (qFuzzyCompare(d->m_nearPlane, nearPlane))
        return;
    d->m_nearPlane = nearPlane;
    emit nearPlaneChanged();
    d->updateProjectionMatrix();
}

float CameraLens::nearPlane() const
{
    Q_D(const CameraLens);
    return d->m_nearPlane;
}

void CameraLens::setFarPlane(float farPlane)
{
    Q_D(CameraLens);
    if (qFuzzyCompare(d->m_farPlane, farPlane))
        return;
    d->m_farPlane = farPlane;
    emit farPlaneChanged();
    d->updateProjectionMatrix();
}

float CameraLens::farPlane() const
{
    Q_D(const CameraLens);
    return d->m_farPlane;
}

void CameraLens::setFieldOfView(float fieldOfView)
{
    Q_D(CameraLens);
    if (qFuzzyCompare(d->m_fieldOfView, fieldOfView))
        return;
    d->m_fieldOfView = fieldOfView;
    emit fieldOfViewChanged();
    d->updateProjectionMatrix();
}

float CameraLens::fieldOfView() const
{
    Q_D(const CameraLens);
    return d->m_fieldOfView;
}

void CameraLens::setAspectRatio(float aspectRatio)
{
    Q_D(CameraLens);
    if (qFuzzyCompare(d->m_aspectRatio, aspectRatio))
        return;
    d->m_aspectRatio = aspectRatio;
    emit aspectRatioChanged();
    d->updateProjectionMatrix();
}

float CameraLens::aspectRatio() const
{
    Q_D(const CameraLens);
    return d->m_aspectRatio;
}

void CameraLens::setLeft(float left)
{
    Q_D(CameraLens);
    if (qFuzzyCompare(d->m_left, left))
        return;
    d->m_left = left;
    emit leftChanged();
    d->updateProjectionMatrix();
}

float CameraLens::left() const
{
    Q_D(const CameraLens);
    return d->m_left;
}

void CameraLens::setRight(float right)
{
    Q_D(CameraLens);
    if (qFuzzyCompare(d->m_right, right))
        return;
    d->m_right = right;
    emit rightChanged();
    d->updateProjectionMatrix();
}

float CameraLens::right() const
{
    Q_D(const CameraLens);
    return d->m_right;
}

void CameraLens::setBottom(float bottom)
{
    Q_D(CameraLens);
    if (qFuzzyCompare(d->m_bottom, bottom))
        return;
    d->m_bottom = bottom;
    emit bottomChanged();
    d->updateProjectionMatrix();
}

float CameraLens::bottom() const
{
    Q_D(const CameraLens);
    return d->m_bottom;
}

void CameraLens::setTop(float top)
{
    Q_D(CameraLens);
    if (qFuzzyCompare(d->m_top, top))
        return;
    d->m_top = top;
    emit topChanged();
    d->updateProjectionMatrix();
}

float CameraLens::top() const
{
    Q_D(const CameraLens);
    return d->m_top;
}

QMatrix4x4 CameraLens::projectionMatrix() const
{
    Q_D(const CameraLens);
    return d->m_projectionMatrix;
}

