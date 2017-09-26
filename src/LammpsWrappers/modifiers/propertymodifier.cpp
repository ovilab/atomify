#include "propertymodifier.h"
#include "../atomdata.h"
#include "../system.h"
#include "../computes.h"
#include "../fixes.h"
#include "../simulatorcontrols/cpcompute.h"
#include "../simulatorcontrols/cpfix.h"
#include "../variables.h"
#include "../simulatorcontrols/cpvariable.h"

PropertyModifier::PropertyModifier()
{

}

void PropertyModifier::applyColors(AtomData &atomData, const std::vector<double> &values, int groupBit) {
    if(m_previousHovered && atomData.paused) return; // No change

    QVector<double> limits = {0, 0.15, 0.35, 0.65, 0.85, 1.0};
    QVector<double> red = {0, 0, 0, 255./255, 255./255, 255./255};
    QVector<double> green = {0, 50./255, 255./255, 255./255, 30./255, 0};
    QVector<double> blue = {100./255, 255./255, 255./255, 0, 0, 0};
    m_cleanPoints.clear();
    m_cleanPoints.reserve(values.size());
    for(double p : values) {
        if(std::isnan(p) || std::isinf(p)) continue;
        m_cleanPoints.push_back(p);
    }

    // Min amd max of dataset
    double min = *std::min_element(std::begin(m_cleanPoints), std::end(m_cleanPoints));
    double max = *std::max_element(std::begin(m_cleanPoints), std::end(m_cleanPoints));

    if(m_previousHovered == nullptr) {
        // We have just hovered a new variable/compute, always use these
        setMin(min);
        setMax(max);
    } else {
        // Keep old values if they are larger/smaller
        double newMin = std::min(min, m_min);
        double newMax = std::max(max, m_max);
        setMin(newMin);
        setMax(newMax);
    }


    double range = m_max - m_min;
    double oneOverRange = 1.0 / range;

    for(int i=0; i<atomData.size(); i++) {
        bool isMemberOfGroup = atomData.bitmask[i] & groupBit; // Each group in LAMMPS is represented as a bit in an int. Bitwise or to check membership.
        if(!isMemberOfGroup) {
            atomData.colors[i][0] = 0.1;
            atomData.colors[i][1] = 0.1;
            atomData.colors[i][2] = 0.1;
            continue;
        }
        double value = values[i];

        if(std::isnan(value) || std::isinf(value)) {
            atomData.colors[i][0] = 1.0;
            atomData.colors[i][1] = 1.0;
            atomData.colors[i][2] = 1.0;
            continue;
        }

        double scaled = (value - m_min) * oneOverRange; // between 0 and 1
        for(int j=0; j<limits.size()-1; j++) {
            if(limits[j] <= scaled && scaled <= limits[j+1]) {
                // Found the two bins including this value
                double dx = limits[j+1] - limits[j];
                double fraction = 1.0 - (scaled - limits[j]) / dx; // where between these two values are we
                atomData.colors[i][0] = red[j]*fraction + (1.0 - fraction)*red[j+1];
                atomData.colors[i][1] = green[j]*fraction + (1.0 - fraction)*green[j+1];
                atomData.colors[i][2] = blue[j]*fraction + (1.0 - fraction)*blue[j+1];
            }
        }
    }
}

void PropertyModifier::apply(AtomData &atomData)
{
    if(!enabled()) {
        setActive(false);
        m_previousHovered = nullptr;
        return;
    }
    QVector<SimulatorControl*> simulatorControls = m_system->simulatorControls();

    for(SimulatorControl *control : simulatorControls) {

        if(control->hovered() && control->isPerAtom()) {

            setActive(true);
            const std::vector<double> &values = control->atomData();
            if(values.size() == atomData.size()) {
                // If we just hovered, the atomData array might not have been updated with values yet
                applyColors(atomData, values, control->groupBit());
                if(control != m_previousHovered) {
                    m_previousHovered = control;
                }
                return;
            }
        }
    }

    setActive(false);
    m_previousHovered = nullptr;
}

bool PropertyModifier::active() const
{
    return m_active;
}

double PropertyModifier::max() const
{
    return m_max;
}

double PropertyModifier::min() const
{
    return m_min;
}

void PropertyModifier::setActive(bool active)
{
    if (m_active == active)
        return;

    m_active = active;
    emit activeChanged(active);
}

void PropertyModifier::setMax(double max)
{
    if (m_max == max)
        return;

    m_max = max;
    emit maxChanged(max);
}

void PropertyModifier::setMin(double min)
{
    if (m_min == min)
        return;

    m_min = min;
    emit minChanged(min);
}
