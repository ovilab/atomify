#ifndef LOOKATTRANSFORM_H
#define LOOKATTRANSFORM_H

#include <QMatrix4x4>

class LookAtTransformPrivate;
class LookAtTransform : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVector3D position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(QVector3D upVector READ upVector WRITE setUpVector NOTIFY upVectorChanged)
    Q_PROPERTY(QVector3D viewCenter READ viewCenter WRITE setViewCenter NOTIFY viewCenterChanged)
    Q_PROPERTY(QVector3D viewVector READ viewVector NOTIFY viewVectorChanged)

public:
    //    explicit LookAtTransform(QNode *parent = 0);
    explicit LookAtTransform(QObject *parent = 0);

//    QMatrix4x4 transformMatrix() const Q_DECL_OVERRIDE;
    QMatrix4x4 transformMatrix() const;

    void setPosition(const QVector3D &position);
    QVector3D position() const;

    void setUpVector(const QVector3D &upVector);
    QVector3D upVector() const;

    void setViewCenter(const QVector3D &viewCenter);
    QVector3D viewCenter() const;

    void setViewVector(const QVector3D &viewVector);
    QVector3D viewVector() const;

Q_SIGNALS:
    void positionChanged();
    void upVectorChanged();
    void viewCenterChanged();
    void viewVectorChanged();
    void transformMatrixChanged();

protected:
//    LookAtTransform(LookAtTransformPrivate &dd, QNode *parent = 0);
//    void copy(const QNode *ref) Q_DECL_OVERRIDE;

private:
//    QT3D_CLONEABLE(LookAtTransform)
    LookAtTransformPrivate* d_ptr;
    Q_DECLARE_PRIVATE(LookAtTransform)
};

#endif // LOOKATTRANSFORM_H
