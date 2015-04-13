#include "camera.h"
#include "camera_p.h"
#include <QDateTime>
#include <QTransform>

CameraPrivate::CameraPrivate(Camera *qq)
//    : QEntityPrivate(qq)
    : q_ptr(qq)
    , m_lens(new CameraLens())
//    , m_transform(new QTransform())
    , m_lookAt(new LookAtTransform())
{
}

//Camera::Camera(QNode *parent) :
//    QEntity(*new CameraPrivate(this), parent)

Camera::Camera(QObject *parent) :
    QObject(parent),
    d_ptr(new CameraPrivate(this))
{
    QObject::connect(d_func()->m_lens, SIGNAL(projectionTypeChanged()), this, SIGNAL(projectionMatrixChanged()));
    QObject::connect(d_func()->m_lens, SIGNAL(nearPlaneChanged()), this, SIGNAL(nearPlaneChanged()));
    QObject::connect(d_func()->m_lens, SIGNAL(farPlaneChanged()), this, SIGNAL(farPlaneChanged()));
    QObject::connect(d_func()->m_lens, SIGNAL(fieldOfViewChanged()), this, SIGNAL(fieldOfViewChanged()));
    QObject::connect(d_func()->m_lens, SIGNAL(aspectRatioChanged()), this, SIGNAL(aspectRatioChanged()));
    QObject::connect(d_func()->m_lens, SIGNAL(leftChanged()), this, SIGNAL(leftChanged()));
    QObject::connect(d_func()->m_lens, SIGNAL(rightChanged()), this, SIGNAL(rightChanged()));
    QObject::connect(d_func()->m_lens, SIGNAL(bottomChanged()), this, SIGNAL(bottomChanged()));
    QObject::connect(d_func()->m_lens, SIGNAL(topChanged()), this, SIGNAL(topChanged()));
    QObject::connect(d_func()->m_lens, SIGNAL(projectionMatrixChanged()), this, SIGNAL(projectionMatrixChanged()));
    QObject::connect(d_func()->m_lookAt, SIGNAL(positionChanged()), this, SIGNAL(positionChanged()));
    QObject::connect(d_func()->m_lookAt, SIGNAL(upVectorChanged()), this, SIGNAL(upVectorChanged()));
    QObject::connect(d_func()->m_lookAt, SIGNAL(viewVectorChanged()), this, SIGNAL(viewVectorChanged()));
    QObject::connect(d_func()->m_lookAt, SIGNAL(viewCenterChanged()), this, SIGNAL(viewCenterChanged()));

    // Default values
    setPosition(QVector3D(0, 0, 5));
    setViewCenter(QVector3D(0, 0, 0));
    setUpVector(QVector3D(0, -1, 0));
    setProjectionType(CameraLens::PerspectiveProjection);
    setFieldOfView(70);
    setNearPlane(0.1);
    setFarPlane(1000.0);
//    QObject::connect(d_func()->m_transform, SIGNAL(matrixChanged()), this, SIGNAL(matrixChanged()));
//    d_func()->m_transform->addTransform(d_func()->m_lookAt);
//    addComponent(d_func()->m_lens);
//    addComponent(d_func()->m_transform);
}

//Camera::Camera(CameraPrivate &dd, QNode *parent)
//    : QEntity(dd, parent)
//{
//    QObject::connect(d_func()->m_lens, SIGNAL(projectionTypeChanged()), this, SIGNAL(projectionMatrixChanged()));
//    QObject::connect(d_func()->m_lens, SIGNAL(nearPlaneChanged()), this, SIGNAL(nearPlaneChanged()));
//    QObject::connect(d_func()->m_lens, SIGNAL(farPlaneChanged()), this, SIGNAL(farPlaneChanged()));
//    QObject::connect(d_func()->m_lens, SIGNAL(fieldOfViewChanged()), this, SIGNAL(fieldOfViewChanged()));
//    QObject::connect(d_func()->m_lens, SIGNAL(aspectRatioChanged()), this, SIGNAL(aspectRatioChanged()));
//    QObject::connect(d_func()->m_lens, SIGNAL(leftChanged()), this, SIGNAL(leftChanged()));
//    QObject::connect(d_func()->m_lens, SIGNAL(rightChanged()), this, SIGNAL(rightChanged()));
//    QObject::connect(d_func()->m_lens, SIGNAL(bottomChanged()), this, SIGNAL(bottomChanged()));
//    QObject::connect(d_func()->m_lens, SIGNAL(topChanged()), this, SIGNAL(topChanged()));
//    QObject::connect(d_func()->m_lens, SIGNAL(projectionMatrixChanged()), this, SIGNAL(projectionMatrixChanged()));
//    QObject::connect(d_func()->m_lookAt, SIGNAL(positionChanged()), this, SIGNAL(positionChanged()));
//    QObject::connect(d_func()->m_lookAt, SIGNAL(upVectorChanged()), this, SIGNAL(upVectorChanged()));
//    QObject::connect(d_func()->m_lookAt, SIGNAL(viewVectorChanged()), this, SIGNAL(viewVectorChanged()));
//    QObject::connect(d_func()->m_lookAt, SIGNAL(viewCenterChanged()), this, SIGNAL(viewCenterChanged()));
//    QObject::connect(d_func()->m_transform, SIGNAL(matrixChanged()), this, SIGNAL(matrixChanged()));
//    d_func()->m_transform->addTransform(d_func()->m_lookAt);
//    addComponent(d_func()->m_lens);
//    addComponent(d_func()->m_transform);
//}

CameraLens *Camera::lens() const
{
    Q_D(const Camera);
    return d->m_lens;
}

//QTransform *Camera::transform() const
//{
//    Q_D(const Camera);
//    return d->m_transform;
//}

LookAtTransform *Camera::lookAt() const
{
    Q_D(const Camera);
    return d->m_lookAt;
}

void Camera::translate( const QVector3D& vLocal, CameraTranslationOption option )
{
    Q_D(Camera);

    // Calculate the amount to move by in world coordinates
    QVector3D vWorld;
    if ( !qFuzzyIsNull( vLocal.x() ) )
    {
        // Calculate the vector for the local x axis
        QVector3D x = QVector3D::crossProduct(d->m_lookAt->viewVector(), d->m_lookAt->upVector()).normalized();
        vWorld += vLocal.x() * x;
    }

    if ( !qFuzzyIsNull( vLocal.y() ) )
        vWorld += vLocal.y() * d->m_lookAt->upVector();

    if ( !qFuzzyIsNull( vLocal.z() ) )
        vWorld += vLocal.z() * d->m_lookAt->viewVector().normalized();

    // Update the camera position using the calculated world vector
    d->m_lookAt->setPosition(d->m_lookAt->position() + vWorld);

    // May be also update the view center coordinates
    if ( option == TranslateViewCenter )
        d->m_lookAt->setViewCenter(d->m_lookAt->viewCenter() + vWorld);

    // Refresh the camera -> view center vector
    d->m_lookAt->setViewVector(d->m_lookAt->viewCenter() - d->m_lookAt->position());

    // Calculate a new up vector. We do this by:
    // 1) Calculate a new local x-direction vector from the cross product of the new
    //    camera to view center vector and the old up vector.
    // 2) The local x vector is the normal to the plane in which the new up vector
    //    must lay. So we can take the cross product of this normal and the new
    //    x vector. The new normal vector forms the last part of the orthonormal basis
    QVector3D x = QVector3D::crossProduct( d->m_lookAt->viewVector(), d->m_lookAt->upVector() ).normalized();
    d->m_lookAt->setUpVector(QVector3D::crossProduct( x, d->m_lookAt->viewVector() ).normalized());
}

void Camera::translateWorld(const QVector3D& vWorld , CameraTranslationOption option )
{
    Q_D(Camera);

    // Update the camera position using the calculated world vector
    d->m_lookAt->setPosition(d->m_lookAt->position() + vWorld);

    // May be also update the view center coordinates
    if ( option == TranslateViewCenter )
        d->m_lookAt->setViewCenter(d->m_lookAt->viewCenter() + vWorld);

    // Refresh the camera -> view center vector
    d->m_lookAt->setViewVector(d->m_lookAt->viewCenter() - d->m_lookAt->position());
}

QQuaternion Camera::tiltRotation(float angle) const
{
    Q_D(const Camera);
    QVector3D xBasis = QVector3D::crossProduct( d->m_lookAt->upVector(),
                                                d->m_lookAt->viewVector().normalized() ).normalized();
    return QQuaternion::fromAxisAndAngle( xBasis, -angle );
}

QQuaternion Camera::panRotation(float angle) const
{
    Q_D(const Camera);
    return QQuaternion::fromAxisAndAngle( d->m_lookAt->upVector(), angle );
}

QQuaternion Camera::rollRotation(float angle) const
{
    Q_D(const Camera);
    return QQuaternion::fromAxisAndAngle( d->m_lookAt->viewVector(), -angle );
}

void Camera::tilt( const float& angle )
{
    QQuaternion q = tiltRotation( angle );
    rotate( q );
}

void Camera::pan( const float& angle )
{
    QQuaternion q = panRotation( -angle );
    rotate( q );
}

void Camera::roll( const float& angle )
{
    QQuaternion q = rollRotation( -angle );
    rotate( q );
}

void Camera::tiltAboutViewCenter( const float& angle )
{
    QQuaternion q = tiltRotation( -angle );
    rotateAboutViewCenter( q );
}

void Camera::panAboutViewCenter( const float& angle )
{
    QQuaternion q = panRotation( angle );
    rotateAboutViewCenter( q );
}

void Camera::rollAboutViewCenter( const float& angle )
{
    QQuaternion q = rollRotation( angle );
    rotateAboutViewCenter( q );
}

void Camera::rotate( const QQuaternion& q )
{
    Q_D(Camera);
    d->m_lookAt->setUpVector(q.rotatedVector(d->m_lookAt->upVector()));
    QVector3D cameraToCenter = q.rotatedVector(d->m_lookAt->viewVector());
    d->m_lookAt->setViewCenter(d->m_lookAt->position() + cameraToCenter);
}

void Camera::rotateAboutViewCenter( const QQuaternion& q )
{
    Q_D(Camera);
    d->m_lookAt->setUpVector(q.rotatedVector(d->m_lookAt->upVector()));
    QVector3D cameraToCenter = q.rotatedVector(d->m_lookAt->viewVector());
    d->m_lookAt->setPosition(d->m_lookAt->viewCenter() - cameraToCenter);
    d->m_lookAt->setViewCenter(d->m_lookAt->position() + cameraToCenter);
}

void Camera::setProjectionType(CameraLens::ProjectionType type)
{
    Q_D(Camera);
    d->m_lens->setProjectionType(type);
}

CameraLens::ProjectionType Camera::projectionType() const
{
    Q_D(const Camera);
    return d->m_lens->projectionType();
}

void Camera::setNearPlane(float nearPlane)
{
    Q_D(Camera);
    d->m_lens->setNearPlane(nearPlane);
}

float Camera::nearPlane() const
{
    Q_D(const Camera);
    return d->m_lens->nearPlane();
}

void Camera::setFarPlane(float farPlane)
{
    Q_D(Camera);
    d->m_lens->setFarPlane(farPlane);
}

float Camera::farPlane() const
{
    Q_D(const Camera);
    return d->m_lens->farPlane();
}

void Camera::setFieldOfView(float fieldOfView)
{
    Q_D(Camera);
    d->m_lens->setFieldOfView(fieldOfView);
}

float Camera::fieldOfView() const
{
    Q_D(const Camera);
    return d->m_lens->fieldOfView();
}

void Camera::setAspectRatio(float aspectRatio)
{
    Q_D(Camera);
    d->m_lens->setAspectRatio(aspectRatio);
}

float Camera::aspectRatio() const
{
    Q_D(const Camera);
    return d->m_lens->aspectRatio();
}

void Camera::setLeft(float left)
{
    Q_D(Camera);
    d->m_lens->setLeft(left);
}

float Camera::left() const
{
    Q_D(const Camera);
    return d->m_lens->left();
}

void Camera::setRight(float right)
{
    Q_D(Camera);
    d->m_lens->setRight(right);
}

float Camera::right() const
{
    Q_D(const Camera);
    return d->m_lens->right();
}

void Camera::setBottom(float bottom)
{
    Q_D(Camera);
    d->m_lens->setBottom(bottom);
}

float Camera::bottom() const
{
    Q_D(const Camera);
    return d->m_lens->bottom();
}

void Camera::setTop(float top)
{
    Q_D(Camera);
    d->m_lens->setTop(top);
}

float Camera::top() const
{
    Q_D(const Camera);
    return d->m_lens->top();
}

QMatrix4x4 Camera::projectionMatrix()
{
    Q_D(const Camera);
    return d->m_lens->projectionMatrix();
}

void Camera::setPosition(const QVector3D &position)
{
    Q_D(Camera);
    d->m_lookAt->setPosition(position);
}

QVector3D Camera::position() const
{
    Q_D(const Camera);
    return d->m_lookAt->position();

}

void Camera::setUpVector(const QVector3D &upVector)
{
    Q_D(Camera);
    d->m_lookAt->setUpVector(upVector);
}

QVector3D Camera::upVector() const
{
    Q_D(const Camera);
    return d->m_lookAt->upVector();
}

void Camera::setViewCenter(const QVector3D &viewCenter)
{
    Q_D(Camera);
    d->m_lookAt->setViewCenter(viewCenter);
}

QVector3D Camera::viewCenter() const
{
    Q_D(const Camera);
    return d->m_lookAt->viewCenter();
}

QVector3D Camera::viewVector() const
{
    Q_D(const Camera);
    return d->m_lookAt->viewVector();
}

QMatrix4x4 Camera::matrix() const
{
    Q_D(const Camera);
    return d->m_lookAt->transformMatrix();
//    return d->m_transform->matrix();
}
