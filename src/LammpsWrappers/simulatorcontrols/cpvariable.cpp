#include "cpvariable.h"
#include <variable.h>
#include <input.h>
#include <library.h>
#include "lammpscontroller.h"
#include "../../dataproviders/data1d.h"
#include "../system.h"
using namespace LAMMPS_NS;

CPVariable::CPVariable(Qt3DCore::QNode *parent) : SimulatorControl(parent),
    m_data(new Data1D(this)), m_value(0), m_isPerAtom(false), m_hovered(false)
{

}

void CPVariable::exportToTextFile(QString fileName)
{
    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        file.setFileName(file.fileName().replace("file://",""));
        if(!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            qDebug() << "Error, could not open file " << fileName;  // TODO: proper error handling
            return;
        }
    }

    QTextStream out(&file);

    // Print header with column names
    out << "# Time " << identifier() << "\n";
    const QList<QPointF> &points = m_data->points();
    for(const QPointF &point : points) {
        out << point.x() << " " << point.y() << "\n";
    }
    file.close();
}

void CPVariable::exportToPythonFile(QString fileName)
{
    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        file.setFileName(file.fileName().replace("file://",""));
        if(!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            qDebug() << "Error, could not open file " << fileName;  // TODO: proper error handling
            return;
        }
    }

    QTextStream out(&file);
    out << "import matplotlib.pyplot as plt\n";
    out << "import numpy as np\n";

    // Print arrays to variables
    const QList<QPointF> &points = m_data->points();
    out << "time = np.asarray([";
    for(auto iter = points.begin(); iter != points.end(); iter++) {
        if (iter != points.begin()) out << ", ";
        out << (*iter).x();
    }
    out << "])\n";

    out << identifier() << " = np.asarray([";
    for(auto iter = points.begin(); iter != points.end(); iter++) {
        if (iter != points.begin()) out << ", ";
        out << (*iter).y();
    }
    out << "])\n\n";

    out << "plt.plot(time, " << identifier() << ")\n";
    out << "plt.xlabel('Time')\n"; // TODO: add units
    out << "plt.ylabel('" << identifier() << "')\n";
    out << "plt.show()\n";

    file.close();
}

void CPVariable::exportToMatlabFile(QString fileName)
{
    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        file.setFileName(file.fileName().replace("file://",""));
        if(!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            qDebug() << "Error, could not open file " << fileName;  // TODO: proper error handling
            return;
        }
    }

    QTextStream out(&file);
    const QList<QPointF> &points = m_data->points();
    out << "time = [";
    for(auto iter = points.begin(); iter != points.end(); iter++) {
        if (iter != points.begin()) out << ", ";
        out << (*iter).x();
    }
    out << "];\n";

    out << identifier() << " = [";
    for(auto iter = points.begin(); iter != points.end(); iter++) {
        if (iter != points.begin()) out << ", ";
        out << (*iter).y();
    }
    out << "];\n";

    out << "plot(time, " << identifier() << ", 'LineWidth', 2);\n";
    out << "set(gca,'fontsize', 16);\n";
    out << "xlabel('Time');\n"; // TODO: add units
    out << "ylabel('" << identifier() << "');\n";

    file.close();
}

void CPVariable::clear()
{
    m_data->clear();
    emit m_data->updated();
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

void CPVariable::updateData1D()
{
    emit m_data->updated();
}

void CPVariable::synchronize(LAMMPSController *lammpsController)
{
    LAMMPS *lammps = lammpsController->lammps();
    Variable *variable = lammps->input->variable;

    QByteArray bytes = identifier().toLocal8Bit();
    int ivar = variable->find(bytes.data());
    if (ivar < 0) return; // Didn't find it. Weird! TODO: handle this
    if (variable->equalstyle(ivar)) {
        double value = variable->compute_equal(ivar);
        double time = lammpsController->system->simulationTime();
        m_data->add(time, value, true);
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
