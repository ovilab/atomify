#ifndef CAMERA_H
#define CAMERA_H
#undef DEGTORAD
#define DEGTORAD M_PI/180.0

#include <QObject>
#include <QVector3D>
#include <QMatrix4x4>
#include <QSize>
#include <QTimer>
#include <QQuaternion>

#include "cameralens.h"
#include "lookattransform.h"

class CameraPrivate;
class QTransform;

class Camera : public QObject
{
    Q_OBJECT
    // CameraLens
    Q_PROPERTY(CameraLens::ProjectionType projectionType READ projectionType WRITE setProjectionType NOTIFY projectionTypeChanged)
    Q_PROPERTY(float nearPlane READ nearPlane WRITE setNearPlane NOTIFY nearPlaneChanged)
    Q_PROPERTY(float farPlane READ farPlane WRITE setFarPlane NOTIFY farPlaneChanged)
    Q_PROPERTY(float fieldOfView READ fieldOfView WRITE setFieldOfView NOTIFY fieldOfViewChanged)
    Q_PROPERTY(float aspectRatio READ aspectRatio WRITE setAspectRatio NOTIFY aspectRatioChanged)
    Q_PROPERTY(float left READ left WRITE setLeft NOTIFY leftChanged)
    Q_PROPERTY(float right READ right WRITE setRight NOTIFY rightChanged)
    Q_PROPERTY(float bottom READ bottom WRITE setBottom NOTIFY bottomChanged)
    Q_PROPERTY(float top READ top WRITE setTop NOTIFY topChanged)
    Q_PROPERTY(QMatrix4x4 projectionMatrix READ projectionMatrix NOTIFY projectionMatrixChanged)
    // LookAt
    Q_PROPERTY(QVector3D position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(QVector3D upVector READ upVector WRITE setUpVector NOTIFY upVectorChanged)
    Q_PROPERTY(QVector3D viewCenter READ viewCenter WRITE setViewCenter NOTIFY viewCenterChanged)
    Q_PROPERTY(QVector3D viewVector READ viewVector NOTIFY viewVectorChanged)
    Q_PROPERTY(QMatrix4x4 matrix READ matrix NOTIFY matrixChanged)

public:
    //    explicit Camera(QNode *parent = 0);
    explicit Camera(QObject *parent = 0);


    enum CameraTranslationOption {
        TranslateViewCenter,
        DontTranslateViewCenter
    };

    CameraLens *lens() const;
    QTransform *transform() const;
    LookAtTransform *lookAt() const;

    QQuaternion tiltRotation(float angle) const;
    QQuaternion panRotation(float angle) const;
    QQuaternion rollRotation(float angle) const;

    // Translate relative to camera orientation axes
    void translate( const QVector3D& vLocal, CameraTranslationOption option = TranslateViewCenter );

    // Translate relative to world axes
    void translateWorld( const QVector3D& vWorld, CameraTranslationOption option = TranslateViewCenter );

    void tilt( const float& angle );
    void pan( const float& angle );
    void roll( const float& angle );

    void tiltAboutViewCenter( const float& angle );
    void panAboutViewCenter( const float& angle );
    void rollAboutViewCenter( const float& angle );

    void rotate( const QQuaternion& q );
    void rotateAboutViewCenter( const QQuaternion& q );

    void setProjectionType(CameraLens::ProjectionType type);
    CameraLens::ProjectionType projectionType() const;

    void setNearPlane(float nearPlane);
    float nearPlane() const;

    void setFarPlane(float farPlane);
    float farPlane() const;

    void setFieldOfView(float fieldOfView);
    float fieldOfView() const;

    void setAspectRatio(float aspectRatio);
    float aspectRatio() const;

    void setLeft(float left);
    float left() const;

    void setRight(float right);
    float right() const;

    void setBottom(float bottom);
    float bottom() const;

    void setTop(float top);
    float top() const;

    QMatrix4x4 projectionMatrix();

    void setPosition(const QVector3D &position);
    QVector3D position() const;

    void setUpVector(const QVector3D &upVector);
    QVector3D upVector() const;

    void setViewCenter(const QVector3D &viewCenter);
    QVector3D viewCenter() const;

    QVector3D viewVector() const;

    QMatrix4x4 matrix() const;

Q_SIGNALS:
    void projectionTypeChanged();
    void nearPlaneChanged();
    void farPlaneChanged();
    void fieldOfViewChanged();
    void aspectRatioChanged();
    void leftChanged();
    void rightChanged();
    void bottomChanged();
    void topChanged();
    void projectionMatrixChanged();
    void positionChanged();
    void upVectorChanged();
    void viewCenterChanged();
    void viewVectorChanged();
    void matrixChanged();

protected:
    CameraPrivate *d_ptr;
    Q_DECLARE_PRIVATE(Camera)
//    Camera(CameraPrivate &dd, QNode *parent = 0);
};

#endif // CAMERA_H
