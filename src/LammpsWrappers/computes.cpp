#include "computes.h"
#include "simulatorcontrols/cpcompute.h"
#include "../lammpscontroller.h"
#include <compute.h>
using namespace LAMMPS_NS;

Computes::Computes(QObject *parent) : QObject(parent)
{

}

void Computes::addCompute(QString identifier, LAMMPSController *lammpsController) {
    if(lammpsController->simulatorControls.contains(identifier)) {
        CPCompute *compute = qobject_cast<CPCompute*>(lammpsController->simulatorControls[identifier]);
        m_data.push_back(compute);
        m_dataMap.insert(identifier, compute);
    } else {
        CPCompute *compute = new CPCompute();
        compute->setIsMirror(true);
        compute->setIdentifier(identifier);
        m_data.push_back(compute);
        m_dataMap.insert(identifier, compute);
    }
}

void Computes::removeCompute(QString identifier) {
    CPCompute *compute = qobject_cast<CPCompute*>(m_dataMap[identifier]);
    if(compute->isMirror()) {
        m_data.removeOne(compute);
        m_dataMap.remove(identifier);
        delete compute;
    } else {
        m_data.removeOne(compute);
        m_dataMap.remove(identifier);
    }
}

void Computes::reset() {
    QList<QObject*> data = m_data;
    for(QObject *object : data) {
        CPCompute *compute = qobject_cast<CPCompute*>(object);
        removeCompute(compute->identifier());
    }
    data.clear();
    setModel(QVariant::fromValue(m_data));
    setCount(0);
}

bool Computes::addOrRemove(LAMMPSController *lammpsController) {
    bool anyChanges = false;
    Modify *modify = lammpsController->lammps()->modify;

    for(int computeIndex=0; computeIndex<modify->ncompute; computeIndex++) {
        Compute *compute = modify->compute[computeIndex];
        QString identifier = QString::fromUtf8(compute->id);

        if(!m_dataMap.contains(identifier)) {
            // New compute we don't have in our list
            anyChanges = true;
            addCompute(identifier, lammpsController);
        }
    }

    QList<CPCompute*> computesToBeRemoved;
    for(QObject *object : m_data) {
        CPCompute *compute = qobject_cast<CPCompute*>(object);
        if(!lammpsController->computeExists(compute->identifier())) {
            // Not in LAMMPS anymore, we should remove it too
            anyChanges = true;
            computesToBeRemoved.push_back(compute);
        }
    }

    for(CPCompute *compute : computesToBeRemoved) {
        removeCompute(compute->identifier());
    }

    return anyChanges;
}

void Computes::updateThreadOnDataObjects(QThread *thread) {
    for(QObject *obj : m_data) {
        CPCompute *compute = qobject_cast<CPCompute*>(obj);
        for(QVariant &variant : compute->data1D()) {
            Data1D *data = variant.value<Data1D *>();
            if(data->thread() != thread) {
                data->moveToThread(thread);
            }
        }
    }
}

void Computes::synchronizeQML(LAMMPSController *lammpsController)
{
    if(!lammpsController->lammps()) return;

    bool anyChanges = addOrRemove(lammpsController);

    if(anyChanges) {
        setModel(QVariant::fromValue(m_data));
        setCount(m_data.size());
    }

    for(QObject *obj : m_data) {
        SimulatorControl *control = qobject_cast<SimulatorControl*>(obj);
        control->updateData1D();
    }
}

void Computes::synchronize(LAMMPSController *lammpsController)
{
    if(!lammpsController->lammps()) { return; }

    computeAll(lammpsController);
    copyAll(lammpsController);
}

void Computes::computeAll(LAMMPSController *lammpsController)
{
    for(QObject *object : m_data) {
        CPCompute *compute = qobject_cast<CPCompute*>(object);
        compute->computeInLAMMPS(lammpsController);
    }
}

void Computes::copyAll(LAMMPSController *lammpsController) {
    for(QObject *object : m_data) {
        CPCompute *compute = qobject_cast<CPCompute*>(object);
        compute->copyData(lammpsController);
    }
}

int Computes::count() const
{
    return m_count;
}

QVariant Computes::model() const
{
    return m_model;
}

QVector<SimulatorControl*> Computes::simulatorControls()
{
    QVector<SimulatorControl*> controls;
    for(QObject *object : m_data) {
        SimulatorControl *control = qobject_cast<SimulatorControl*>(object);
        controls.append(control);
    }
    return controls;
}

void Computes::setCount(int count)
{
    if (m_count == count)
        return;

    m_count = count;
    emit countChanged(count);
}

void Computes::setModel(QVariant model)
{
    if (m_model == model)
        return;

    m_model = model;
    emit modelChanged(model);
}
