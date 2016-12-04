#include "cpvariable.h"
#include <variable.h>
#include <input.h>
#include "lammpscontroller.h"
#include "../../dataproviders/data1d.h"
#include "../system.h"
using namespace LAMMPS_NS;

CPVariable::CPVariable(Qt3DCore::QNode *parent) : SimulatorControl(parent),
    m_data(new Data1D(this)), m_value(0), m_isPerAtom(false), m_hovered(false)
{

}



void CPVariable::updateCommand()
{
}

QList<QString> CPVariable::enabledCommands()
{
    return { fullCommand() };
}

QList<QString> CPVariable::disableCommands()
{
    return {QString("variable %1 delete").arg(identifier())};
}

QList<QString> CPVariable::resetCommands()
{
    return { QString("variable %1 delete").arg(identifier()), QString("variable %1 %2").arg(identifier()).arg(command()) };
}

QString CPVariable::createCommandPrefix()
{
    return QString("variable %1 ").arg(identifier());
}

const std::vector<double> &CPVariable::atomData() const
{
    return m_atomData;
}

bool CPVariable::existsInLammps(LAMMPSController *lammpsController)
{
    return lammpsController->findVariableIndex(identifier())>=0;
}

void CPVariable::synchronize(LAMMPSController *lammpsController)
{
    LAMMPS *lammps = lammpsController->lammps();
    Variable *variable = lammps->input->variable;
    Error *error = lammps->error;

    try {
        QByteArray bytes = identifier().toLocal8Bit();
        int ivar = variable->find(bytes.data());
        if (ivar < 0) return; // Didn't find it. Weird! TODO: handle this

        if (variable->equalstyle(ivar)) {
            double value = variable->compute_equal(ivar);
            double time = lammpsController->system->simulationTime();
            m_data->add(time, value, false);
            setValue(value);
            setValueHasDecimals(value!=int(value));
        }

        if (variable->atomstyle(ivar)) {
            int igroup = 0;
            m_atomData.resize(lammpsController->system->numberOfAtoms());
            double *vector = &m_atomData.front();
            variable->compute_atom(ivar,igroup,vector,1,0);
            setIsPerAtom(true);
            m_data->createHistogram(m_atomData);
        }
    } catch(LAMMPSAbortException & ae) {
        qDebug() << "Yeah didn't go so well: " << ae.message.c_str();
        error->set_last_error(ae.message.c_str(), ERROR_NORMAL);
    } catch(LAMMPSException & e) { \
        qDebug() << "Yeah didn't go so well: " << e.message.c_str();
        error->set_last_error(e.message.c_str(), ERROR_NORMAL);
    }

}

Data1D *CPVariable::data() const
{
    return m_data;
}

double CPVariable::value() const
{
    return m_value;
}

bool CPVariable::valueHasDecimals() const
{
    return m_valueHasDecimals;
}

bool CPVariable::isPerAtom() const
{
    return m_isPerAtom;
}

bool CPVariable::hovered() const
{
    return m_hovered;
}

void CPVariable::setData(Data1D *data)
{
    if (m_data == data)
        return;

    m_data = data;
    emit dataChanged(data);
}

void CPVariable::setValue(double value)
{
    if (m_value == value)
        return;

    m_value = value;
    emit valueChanged(value);
}

void CPVariable::setValueHasDecimals(bool valueHasDecimals)
{
    if (m_valueHasDecimals == valueHasDecimals)
        return;

    m_valueHasDecimals = valueHasDecimals;
    emit valueHasDecimalsChanged(valueHasDecimals);
}

void CPVariable::setIsPerAtom(bool isPerAtom)
{
    if (m_isPerAtom == isPerAtom)
        return;

    m_isPerAtom = isPerAtom;
    emit isPerAtomChanged(isPerAtom);
}

void CPVariable::setHovered(bool hovered)
{
    if (m_hovered == hovered)
        return;

    m_hovered = hovered;
    emit hoveredChanged(hovered);
}
