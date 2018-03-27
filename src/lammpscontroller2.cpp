#include "lammpscontroller2.h"
#include <QDebug>

namespace atomify {

LAMMPSController2::LAMMPSController2(Qt3DCore::QNode *parent)
    : Qt3DCore::QComponent(parent)
{
    qDebug() << "Yeah baby";
}

}
