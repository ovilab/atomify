#ifndef TMPLAMMPS_H
#define TMPLAMMPS_H

#include <functional>

namespace atomify {

class TMPLAMMPS
{
public:
    int timestep = 0;
    TMPLAMMPS();
    void run();
    std::function<void(TMPLAMMPS *)> m_callback;
    void setCallback(const std::function<void (TMPLAMMPS *)> &callback);
};

}

#endif // TMPLAMMPS_H
