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
}

void Computes::synchronize(LAMMPSController *lammpsController)
{
    LAMMPS *lammps = lammpsController->lammps();
    if(!lammps || !lammps->modify) {
        reset();
        return;
    }

    Modify *modify = lammps->modify;
    int numComputes = modify->ncompute;
    setCount(numComputes);

    for(int computeIndex=0; computeIndex<numComputes; computeIndex++) {
        Compute *compute = modify->compute[computeIndex];
        QString identifier = QString::fromUtf8(compute->id);

        if(!m_dataMap.contains(identifier)) {
            addCompute(identifier, lammpsController);
        }
    }

    QList<CPCompute*> computesToBeRemoved;
    for(QObject *object : m_data) {
        CPCompute *compute = qobject_cast<CPCompute*>(object);
        if(!lammpsController->computeExists(compute->identifier())) {
            // Not in LAMMPS anymore, we should remove it too
            computesToBeRemoved.push_back(compute);
        }
    }

    for(CPCompute *compute : computesToBeRemoved) {
        removeCompute(compute->identifier());
    }

    for(QObject *obj : m_data) {
        CPCompute *compute = qobject_cast<CPCompute*>(obj);
        for(QVariant &variant : compute->data1D()) {
            CP1DData *data = variant.value<CP1DData *>();
            emit data->updated();
        }
    }

    if(!lammpsController->state.canProcessSimulatorControls) return;
    for(QObject *object : m_data) {
        CPCompute *compute = qobject_cast<CPCompute*>(object);
        compute->copyData(lammpsController);
    }

    setCount(m_data.size());
    setModel(QVariant::fromValue(m_data));
}

void Computes::computeAll(LAMMPSController *lammpsController)
{
    for(QObject *object : m_data) {
        CPCompute *compute = qobject_cast<CPCompute*>(object);
        compute->computeInLAMMPS(lammpsController);
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

bool Computes::active() const
{
    return m_active;
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

void Computes::setActive(bool active)
{
    if (m_active == active)
        return;

    m_active = active;
    emit activeChanged(active);
}
