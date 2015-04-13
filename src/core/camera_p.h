#ifndef CAMERA_P_H
#define CAMERA_P_H

#include <QObject>
#include "lookattransform.h"
#include "cameralens.h"

class Camera;
class CameraPrivate // : public QEntityPrivate
{
public:
    CameraPrivate(Camera *qq);

    Q_DECLARE_PUBLIC(Camera)

    CameraLens *m_lens;
//    QTransform *m_transform;
    LookAtTransform *m_lookAt;

    Camera *q_ptr;
};

#endif // CAMERA_P_H

