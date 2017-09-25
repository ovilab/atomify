#include "simulatorcontrol.h"
#include "lammpscontroller.h"
#include "mysimulator.h"

SimulatorControl::SimulatorControl(Qt3DCore::QNode *parent) : Qt3DCore::QNode(parent)
{

}

int SimulatorControl::getNextId()
{
    static int nextId = 0;
    return nextId++;
}

SimulatorControl::~SimulatorControl()
{
    emit willBeDestroyed();
    m_dependencies.clear();
}

bool SimulatorControl::addToLammps(LAMMPSController *lammpsController) {
//    for(const QVariant &variant : m_dependencies) {
//        SimulatorControl *dependency = qvariant_cast<SimulatorControl*>(variant);
//        if(dependency && !dependency->existsInLammps(lammpsController)) {
//            // We found one dependency that is not added to LAMMPS, abort this mission
//            return false;
//        }
//    }

    return true;
}

void SimulatorControl::addDependency(SimulatorControl *control)
{
    bool found = false;
    for(QVariant &variant : m_dependencies) {
        SimulatorControl *thisControl = variant.value<SimulatorControl*>();
        if(thisControl==control) {
            found = true;
            break;
        }
    }

    if(!found) {
        m_dependencies.push_back(QVariant::fromValue(control));
    }
}

void SimulatorControl::removeDependency(SimulatorControl *control)
{
    for(QVariant &variant : m_dependencies) {
        SimulatorControl *thisControl = variant.value<SimulatorControl*>();
        if(thisControl==control) {
            m_dependencies.removeOne(variant);
            break;
        }
    }
}

bool SimulatorControl::dependenciesValid(LAMMPSController *lammpsController)
{
    bool valid = true;
    for(const QVariant &variant : m_dependencies) {
        SimulatorControl *dependency = qvariant_cast<SimulatorControl*>(variant);
        if(dependency) {
            // Check if the dependency is not in lammps or one of its dependencies are not in lammps
            if(!dependency->existsInLammps(lammpsController) || !dependency->dependenciesValid(lammpsController)) {
                valid = false;
            }
        }
    }

    return valid;
}

void SimulatorControl::update(LAMMPSController *lammpsController)
{
    if(m_isMirror) return; // If this object is only a mirror object, we shouldn't mess with adding or removing things from LAMMPS.

//    bool exists = existsInLammps(lammpsController);
//    if(!exists && m_enabled) {
//        // We should exist, so let's try to add.
//        // Whatever happens, just return. We aren't ready to compute any values yet anyway.
//        addToLammps(lammpsController);
//        return;
//    }

//    if(exists && !m_enabled || !dependenciesValid(lammpsController)) {
//        // We should not exist, but we do. Now remove from lammps
//        lammpsController->scriptHandler()->addCommandsToTop(disableCommands(), ScriptCommand::Type::SingleCommand);
//    }

//    if(exists) {
//        QString currentCommand = command();
//        updateCommand();
//        if(currentCommand!=command()) {
//            lammpsController->scriptHandler()->addCommandsToTop(resetCommands(), ScriptCommand::Type::SingleCommand);
//        }
//    }
}

void SimulatorControl::handleCommand(QString command) { /* TODO */ }

QString SimulatorControl::fullCommand()
{
    return QString("%1 %2").arg(createCommandPrefix()).arg(command());
}

bool SimulatorControl::isMirror() const
{
    return m_isMirror;
}

QVariantMap SimulatorControl::data1D() const
{
    return m_data1D;
}

bool SimulatorControl::hasScalarData() const
{
    return m_hasScalarData;
}

float SimulatorControl::scalarValue() const
{
    return m_scalarValue;
}

QString SimulatorControl::group() const
{
    return m_group;
}

bool SimulatorControl::isPerAtom() const
{
    return m_isPerAtom;
}

int SimulatorControl::perAtomIndex() const
{
    return m_perAtomIndex;
}

int SimulatorControl::numPerAtomValues() const
{
    return m_numPerAtomValues;
}

int SimulatorControl::frequency() const
{
    return m_frequency;
}

QString SimulatorControl::xLabel() const
{
    return m_xLabel;
}

QString SimulatorControl::yLabel() const
{
    return m_yLabel;
}

bool SimulatorControl::interactive() const
{
    return m_interactive;
}

bool SimulatorControl::hovered() const
{
    return m_hovered;
}

bool SimulatorControl::enabled() const
{
    return m_enabled;
}

QString SimulatorControl::identifier() const
{
    return m_identifierPrefix+m_identifier;
}

QString SimulatorControl::command() const
{
    return m_command;
}

QVariantList SimulatorControl::dependencies() const
{
    return m_dependencies;
}

void SimulatorControl::setEnabled(bool enabled)
{
    if (m_enabled == enabled)
        return;
    m_enabled = enabled;
    emit enabledChanged(enabled);
}

void SimulatorControl::setIdentifier(QString identifier)
{
    if(!isMirror()) identifier = identifier+QString("%1").arg(SimulatorControl::getNextId());
    if (m_identifier == identifier)
        return;

    m_identifier = identifier;
    emit identifierChanged(identifier);
}

void SimulatorControl::setCommand(QString command)
{
    if (m_command == command)
        return;

    m_command = command;
    emit commandChanged(command);
}

void SimulatorControl::setDependencies(QVariantList dependencies)
{
    if (m_dependencies == dependencies)
        return;

    m_dependencies = dependencies;
    emit dependenciesChanged(dependencies);
}

void SimulatorControl::setIsMirror(bool isMirror)
{
    if (m_isMirror == isMirror)
        return;

    m_isMirror = isMirror;
    if(m_isMirror) {
        m_identifierPrefix = "";
    } else {
        m_identifierPrefix = "atomify_";
    }
    emit isMirrorChanged(isMirror);
}

void SimulatorControl::setData1D(QVariantMap data1D)
{
    if (m_data1D == data1D)
        return;

    m_data1D = data1D;
    emit data1DChanged(m_data1D);
}

void SimulatorControl::setHasScalarData(bool hasScalarData)
{
    if (m_hasScalarData == hasScalarData)
        return;

    m_hasScalarData = hasScalarData;
    emit hasScalarDataChanged(m_hasScalarData);
}

void SimulatorControl::setScalarValue(float scalarValue)
{
    if (qFuzzyCompare(m_scalarValue, scalarValue))
        return;

    m_scalarValue = scalarValue;
    emit scalarValueChanged(m_scalarValue);
}

void SimulatorControl::setGroup(QString group)
{
    if (m_group == group)
        return;

    m_group = group;
    emit groupChanged(m_group);
}

void SimulatorControl::setIsPerAtom(bool isPerAtom)
{
    if (m_isPerAtom == isPerAtom)
        return;

    m_isPerAtom = isPerAtom;
    emit isPerAtomChanged(m_isPerAtom);
}

void SimulatorControl::setPerAtomIndex(int perAtomIndex)
{
    if (m_perAtomIndex == perAtomIndex)
        return;

    m_perAtomIndex = perAtomIndex;
    emit perAtomIndexChanged(m_perAtomIndex);
}

void SimulatorControl::setNumPerAtomValues(int numPerAtomValues)
{
    if (m_numPerAtomValues == numPerAtomValues)
        return;

    m_numPerAtomValues = numPerAtomValues;
    emit numPerAtomValuesChanged(m_numPerAtomValues);
}

void SimulatorControl::setFrequency(int frequency)
{
    if (m_frequency == frequency)
        return;

    m_frequency = frequency;
    emit frequencyChanged(m_frequency);
}

void SimulatorControl::setXLabel(QString xLabel)
{
    if (m_xLabel == xLabel)
        return;

    m_xLabel = xLabel;
    emit xLabelChanged(m_xLabel);
}

void SimulatorControl::setYLabel(QString yLabel)
{
    if (m_yLabel == yLabel)
        return;

    m_yLabel = yLabel;
    emit yLabelChanged(m_yLabel);
}

void SimulatorControl::setInteractive(bool interactive)
{
    if (m_interactive == interactive)
        return;

    m_interactive = interactive;
    emit interactiveChanged(m_interactive);
}

void SimulatorControl::setHovered(bool hovered)
{
    if (m_hovered == hovered)
        return;

    m_hovered = hovered;
    emit hoveredChanged(m_hovered);
}

void SimulatorControl::setType(QString type)
{
    if (m_type == type)
            return;

        m_type = type;
        emit typeChanged(m_type);
}

Data1D *SimulatorControl::ensureExists(QString key, bool enabledByDefault) {
    if(!m_data1DRaw.contains(key)) {
        Data1D *data = new Data1D();
        data->setEnabled(enabledByDefault);
        m_data1DRaw.insert(key, data);
        m_data1D.insert(key, QVariant::fromValue<Data1D*>(data));
    }
    return m_data1DRaw[key];
}

void SimulatorControl::updateData1D()
{
    for(QVariant &variant : data1D()) {
        Data1D *data = variant.value<Data1D *>();
        if(data->isHistogram()) {
            emit data->updatedHistogram(data);
        } else {
            emit data->updated();
        }
    }
}

const std::vector<double> &SimulatorControl::atomData() const
{
    return m_atomData;
}

QString SimulatorControl::type() const
{
    return m_type;
}

int SimulatorControl::groupBit() const
{
    return m_groupBit;
}
