#include "lookattransform.h"
#include "lookattransform_p.h"

LookAtTransformPrivate::LookAtTransformPrivate(LookAtTransform *qq)
//    : QAbstractTransformPrivate(qq)
    : q_ptr(qq)
    , m_matrixDirty(true)
{
}

//LookAtTransform::LookAtTransform(QNode *parent)
//    : QAbstractTransform(*new LookAtTransformPrivate(this), parent)
//{
//}

LookAtTransform::LookAtTransform(QObject *parent)
    : QObject(parent)
    , d_ptr(new LookAtTransformPrivate(this))
{
}

//void LookAtTransform::copy(const QNode *ref)
//{
//    QAbstractTransform::copy(ref);
//    const LookAtTransform *transform = static_cast<const LookAtTransform*>(ref);
//    d_func()->m_matrix = transform->d_func()->m_matrix;
//    d_func()->m_position = transform->d_func()->m_position;
//    d_func()->m_upVector = transform->d_func()->m_upVector;
//    d_func()->m_viewCenter = transform->d_func()->m_viewCenter;
//    d_func()->m_viewVector = transform->d_func()->m_viewVector;
//}

//LookAtTransform::LookAtTransform(LookAtTransformPrivate &dd, QNode *parent)
//    : QAbstractTransform(dd, parent)
//{
//}

QMatrix4x4 LookAtTransform::transformMatrix() const
{
    Q_D(const LookAtTransform);
    if (d->m_matrixDirty) {
        d->m_matrix.setToIdentity();
        d->m_matrix.lookAt(d->m_position, d->m_viewCenter, d->m_upVector);
        d->m_matrixDirty = false;
    }
    return d->m_matrix;
}

QVector3D LookAtTransform::position() const
{
    Q_D(const LookAtTransform);
    return d->m_position;
}

void LookAtTransform::setPosition(const QVector3D &position)
{
    Q_D(LookAtTransform);
    if (d->m_position != position) {
        d->m_position = position;
        d->m_viewVector = d->m_viewCenter - position;
        d->m_matrixDirty = true;
        emit positionChanged();
        emit viewVectorChanged();
        emit transformMatrixChanged();
    }
}

void LookAtTransform::setUpVector(const QVector3D &upVector)
{
    Q_D(LookAtTransform);
    if (d->m_upVector != upVector) {
        d->m_upVector = upVector;
        d->m_matrixDirty = true;
        emit upVectorChanged();
        emit transformMatrixChanged();
    }
}

QVector3D LookAtTransform::upVector() const
{
    Q_D(const LookAtTransform);
    return d->m_upVector;
}

void LookAtTransform::setViewCenter(const QVector3D &viewCenter)
{
    Q_D(LookAtTransform);
    if (d->m_viewCenter != viewCenter) {
        d->m_viewCenter = viewCenter;
        d->m_viewVector = viewCenter - d->m_position;
        d->m_matrixDirty = true;
        emit viewCenterChanged();
        emit viewVectorChanged();
        emit transformMatrixChanged();
    }
}

QVector3D LookAtTransform::viewCenter() const
{
    Q_D(const LookAtTransform);
    return d->m_viewCenter;
}

void LookAtTransform::setViewVector(const QVector3D &viewVector)
{
    Q_D(LookAtTransform);
    if (!qFuzzyCompare(d->m_viewVector, viewVector)) {
        d->m_viewVector = viewVector;
        // modify the view center, not the position
        d->m_viewCenter = d->m_position + viewVector;
        d->m_matrixDirty = true;
        emit viewVectorChanged();
        emit viewCenterChanged();
        emit transformMatrixChanged();
    }
}

QVector3D LookAtTransform::viewVector() const
{
    Q_D(const LookAtTransform);
    return d->m_viewVector;
}

