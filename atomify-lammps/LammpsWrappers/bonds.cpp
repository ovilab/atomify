#include "bonds.h"

Bonds::Bonds()
{
    reset();
}

bool Bonds::enabled() const
{
    bool anyActiveBonds = false;
    for(const QVector<float> &vec : m_bondLengths) {
        for(const float &length : vec) {
            if(length > 0.0) {
                anyActiveBonds = true;
                break;
            }
        }
    }
    return m_enabled && anyActiveBonds;
}

QVector<QVector<float> > &Bonds::bondLengths()
{
    return m_bondLengths;
}

float Bonds::maxBondLength()
{
    float maxBondLength = 0;
    for(const QVector<float> &vec : m_bondLengths) {
        for(const float &length : vec) {
            maxBondLength = std::max(length, maxBondLength);
        }
    }
    return maxBondLength;
}

void Bonds::reset()
{
    int numTypes = 32;
    m_bondLengths.resize(numTypes);
    for(QVector<float> &vec : m_bondLengths) {
        vec.resize(numTypes);
        for(float &length : vec) {
            length = 0.0;
        }
    }

    m_enabled = true;
}

void Bonds::setEnabled(bool enabled)
{
    if (m_enabled == enabled)
        return;

    m_enabled = enabled;
    emit enabledChanged(enabled);
}
