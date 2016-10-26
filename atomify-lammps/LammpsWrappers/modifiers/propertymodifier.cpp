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

void PropertyModifier::applyColors(AtomData &atomData, const std::vector<double> &values) {
    QVector<double> limits = {0, 0.15, 0.35, 0.65, 0.85, 1.0};
    QVector<double> red = {0, 0, 0, 255./255, 255./255, 100./255};
    QVector<double> green = {0, 50./255, 255./255, 255./255, 30./255, 0};
    QVector<double> blue = {100./255, 255./255, 255./255, 0, 0, 0};

    double min = *std::min_element(std::begin(values), std::end(values));
    double max = *std::max_element(std::begin(values), std::end(values));
    double range = max - min;
    double oneOverRange = 1.0 / range;

    for(int i=0; i<atomData.size(); i++) {
        double value = values[i];
        double scaled = (value - min) * oneOverRange; // between 0 and 1
        for(int j=0; j<limits.size()-1; j++) {
            if(limits[j] <= scaled && scaled < limits[j+1]) {
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
            const std::vector<double> &values = compute->atomData();
            applyColors(atomData, values);
        }
    }

    QVector<CPVariable*> variables = m_system->variables()->variables();
    for(CPVariable *variable : variables) {
        if(variable->hovered() && variable->isPerAtom()) {
            const std::vector<double> &values = variable->atomData();
            applyColors(atomData, values);
        }
    }

}
