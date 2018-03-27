#ifndef TMPLAMMPS_H
#define TMPLAMMPS_H

#include <QVector>
#include <QVector3D>
#include <functional>

namespace atomify {

class TMPLAMMPS
{
public:
    int timestep = 0;
    QVector<QVector3D> positions;
    TMPLAMMPS();
    void run();
    std::function<void(TMPLAMMPS *)> m_callback;
    void setCallback(const std::function<void (TMPLAMMPS *)> &callback);
};

}

#endif // TMPLAMMPS_H
