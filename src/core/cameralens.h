#ifndef CAMERALENS_H
#define CAMERALENS_H

#include <QObject>
#include <QMatrix4x4>

class CameraLensPrivate;

class CameraLens : public QObject
{
    Q_OBJECT

    Q_ENUMS( ProjectionType )
    Q_PROPERTY(ProjectionType projectionType READ projectionType WRITE setProjectionType NOTIFY projectionTypeChanged)
    Q_PROPERTY(float nearPlane READ nearPlane WRITE setNearPlane NOTIFY nearPlaneChanged)
    Q_PROPERTY(float farPlane READ farPlane WRITE setFarPlane NOTIFY farPlaneChanged)
    Q_PROPERTY(float fieldOfView READ fieldOfView WRITE setFieldOfView NOTIFY fieldOfViewChanged)
    Q_PROPERTY(float aspectRatio READ aspectRatio WRITE setAspectRatio NOTIFY aspectRatioChanged)
    Q_PROPERTY(float left READ left WRITE setLeft NOTIFY leftChanged)
    Q_PROPERTY(float right READ right WRITE setRight NOTIFY rightChanged)
    Q_PROPERTY(float bottom READ bottom WRITE setBottom NOTIFY bottomChanged)
    Q_PROPERTY(float top READ top WRITE setTop NOTIFY topChanged)
    Q_PROPERTY(QMatrix4x4 projectionMatrix READ projectionMatrix NOTIFY projectionMatrixChanged)

public:
//    explicit CameraLens(QNode *parent = 0);
    explicit CameraLens(QObject *parent = 0);

    enum ProjectionType {
        OrthogonalProjection,
        PerspectiveProjection
    };

    void setProjectionType(ProjectionType projectionType);
    ProjectionType projectionType() const;

    void setNearPlane(float nearPlane);
    float nearPlane() const;

    void setFarPlane(float nearPlane);
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

    QMatrix4x4 projectionMatrix() const;

    void setOrthographicProjection(float left, float right,
                                   float bottom, float top,
                                   float nearPlane, float farPlane);

    void setPerspectiveProjection(float fieldOfView, float aspect,
                                  float nearPlane, float farPlane);

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

protected:
//    CameraLens(CameraLensPrivate &dd, QNode *parent = 0);
//    void copy(const QNode *ref) Q_DECL_OVERRIDE;

private:
    CameraLensPrivate* d_ptr;
    Q_DECLARE_PRIVATE(CameraLens)
//    QT3D_CLONEABLE(CameraLens)
};

#endif // CAMERALENS_H
