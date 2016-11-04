#include "propertymodifier.h"
#include "../atomdata.h"
#include "../system.h"
#include "../computes.h"
#include "../simulatorcontrols/cpcompute.h"
#include "../variables.h"
#include "../simulatorcontrols/cpvariable.h"

PropertyModifier::PropertyModifier()
{

}

void PropertyModifier::applyColors(AtomData &atomData, const std::vector<double> &values, int groupBit) {
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

    double min = *std::min_element(std::begin(m_cleanPoints), std::end(m_cleanPoints));
    double max = *std::max_element(std::begin(m_cleanPoints), std::end(m_cleanPoints));
    setMin(min);
    setMax(max);
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
    if(!enabled()) return;
    QVector<CPCompute*> computes = m_system->computes()->computes();
    for(CPCompute *compute : computes) {
        if(compute->hovered() && compute->isPerAtom()) {
            setActive(true);
            const std::vector<double> &values = compute->atomData();
            if(values.size() == atomData.size()) {
                applyColors(atomData, values, compute->groupBit());
                return;
            }
        }
    }

    QVector<CPVariable*> variables = m_system->variables()->variables();
    for(CPVariable *variable : variables) {
        if(variable->hovered() && variable->isPerAtom()) {
            const std::vector<double> &values = variable->atomData();
            if(values.size() == atomData.size()) {
                applyColors(atomData, values, 1); // groupBit = 1 will be true for all bitwise (i.e. all groups)
                setActive(true);
                return;
            }
        }
    }
    setActive(false);
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
