#ifndef LOOKATTRANSFORM_P_H
#define LOOKATTRANSFORM_P_H

#include <QObject>

class LookAtTransform;
class LookAtTransformPrivate
{
public :
    LookAtTransformPrivate(LookAtTransform *qq);

    Q_DECLARE_PUBLIC(LookAtTransform)

    mutable QMatrix4x4 m_matrix;
    QVector3D m_position;
    QVector3D m_upVector;
    QVector3D m_viewCenter;
    QVector3D m_viewVector; // From "camera" position to view center
    mutable bool m_matrixDirty;

    LookAtTransform* q_ptr;
};

#endif // LOOKATTRANSFORM_P_H

