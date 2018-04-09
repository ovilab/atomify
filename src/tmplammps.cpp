#include "tmplammps.h"
#include <QThread>

namespace atomify {

TMPLAMMPS::TMPLAMMPS()
{
    positions.resize(10);
}

void TMPLAMMPS::run()
{
    for (int i = 0; i < 10; i++) {
        timestep++;
        for (auto& v : positions) {
            v[0] += 1;
        }

        QThread::msleep(1000);
        if (m_callback) {
            m_callback(this);
        }
    }
}

void TMPLAMMPS::setCallback(const std::function<void(TMPLAMMPS*)>& callback)
{
    m_callback = callback;
}
}
