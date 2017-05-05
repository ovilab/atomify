#include "variables.h"
#include "lammpscontroller.h"
#include "simulatorcontrols/cpvariable.h"
#include <QDebug>
#include <mpi.h>
#include <input.h>
#include <update.h>
#include <info.h>
#include <lammps.h>
#include <variable.h>
#include <modify.h>
using namespace LAMMPS_NS;

Variables::Variables(QObject *parent) : QObject(parent)
{

}

bool Variables::isSupported(LAMMPSController *lammpsController, int ivar)
{
    return lammpsController->lammps()->input->variable->equalstyle(ivar) || lammpsController->lammps()->input->variable->atomstyle(ivar);
}

bool Variables::addOrRemove(LAMMPSController *lammpsController)
{
    LAMMPS *lammps = lammpsController->lammps();
    Variable *variable = lammps->input->variable;
    int nvar;
    Info info(lammps);
    char **names = info.get_variable_names(nvar);
    bool anyChanges = false;
    if(names == nullptr) return false; // Maybe next time

    // First loop through all variables in LAMMPS to find new variables
    for(int i=0; i<nvar; i++) {
        char *name = names[i];
        int ivar = variable->find(name);
        if(ivar<0) continue; // Should never happen since we got the list from lammps just now, but prevents segfaults.
        if(isSupported(lammpsController, ivar)) {
            QString identifier = QString::fromUtf8(name);
            if(!m_dataMap.contains(identifier)) {
                // New variable in LAMMPS, add it
                anyChanges = true;
                add(identifier, lammpsController);
            }
        }
    }

    // Search for variables in our list that are removed from LAMMPS
    QList<CPVariable*> toBeRemoved;
    for(QObject *object : m_data) {
        CPVariable *variable = qobject_cast<CPVariable*>(object);
        if(!lammpsController->variableExists(variable->identifier())) {
            // Not in LAMMPS anymore, we should remove it too
            anyChanges = true;
            toBeRemoved.push_back(variable);
        }
    }

    // Remove these from our list
    for(CPVariable *variable : toBeRemoved) {
        remove(variable->identifier());
    }

    return anyChanges;
}

void Variables::synchronizeQML(LAMMPSController *lammpsController)
{
    if(!lammpsController->lammps()) return;
    bool anyChanges = addOrRemove(lammpsController);

    if(anyChanges) {
        setModel(QVariant::fromValue(m_data));
        setCount(m_data.size());
    }

    for(QObject *obj : m_data) {
        CPVariable *variable = qobject_cast<CPVariable*>(obj);
        variable->updateData1D();
    }
}

void Variables::copyData(LAMMPSController *lammpsController) {
    for(QObject *object : m_data) {
        CPVariable *variable = qobject_cast<CPVariable*>(object);
        variable->synchronize(lammpsController);
    }
}

void Variables::synchronize(LAMMPSController *lammpsController)
{
    if(!lammpsController->lammps()) return;
    copyData(lammpsController);
}

int Variables::count() const
{
    return m_count;
}

QVariant Variables::model() const
{
    return m_model;
}

QVector<CPVariable *> Variables::variables()
{
    QVector<CPVariable *> variables;
    for(QObject *object : m_data) {
        CPVariable *variable = qobject_cast<CPVariable*>(object);
        variables.append(variable);
    }
    return variables;
}

void Variables::setCount(int count)
{
    if (m_count == count)
        return;

    m_count = count;
    emit countChanged(count);
}

void Variables::setModel(QVariant model)
{
    if (m_model == model)
        return;

    m_model = model;
    emit modelChanged(model);
}

void Variables::add(QString identifier, LAMMPSController *lammpsController)
{
    if(lammpsController->simulatorControls.contains(identifier)) {
        CPVariable *variable = qobject_cast<CPVariable*>(lammpsController->simulatorControls[identifier]);
        m_data.push_back(variable);
        m_dataMap.insert(identifier, variable);
    } else {
        CPVariable *variable = new CPVariable();
        variable->setIsMirror(true);
        variable->setIdentifier(identifier);
        m_data.push_back(variable);
        m_dataMap.insert(identifier, variable);
    }
}

void Variables::remove(QString identifier)
{
    CPVariable *variable = qobject_cast<CPVariable*>(m_dataMap[identifier]);
    if(variable->isMirror()) {
        m_data.removeOne(variable);
        m_dataMap.remove(identifier);
        delete variable;
    } else {
        m_data.removeOne(variable);
        m_dataMap.remove(identifier);
    }
}

void Variables::reset()
{
    QList<QObject*> data = m_data;
    for(QObject *object : data) {
        CPVariable *variable = qobject_cast<CPVariable*>(object);
        remove(variable->identifier());
    }
    data.clear();
    setModel(QVariant::fromValue(m_data));
    setCount(0);
}
