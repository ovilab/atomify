#include "variables.h"
#include <mpi.h>
#include <lammps.h>
#include <variable.h>
#include <modify.h>
using namespace LAMMPS_NS;

Variables::Variables(QObject *parent) : QObject(parent)
{

}

void Variables::synchronize(LAMMPSController *lammpsController)
{
//    LAMMPS *lammps = lammpsController->lammps();
//    if(!lammps) {
//        reset();
//        return;
//    }

//    Modify *modify = lammps->modify;
//    int numComputes = modify->ncompute;
//    bool anyChanges = false;
//    for(int computeIndex=0; computeIndex<numComputes; computeIndex++) {
//        Compute *compute = modify->compute[computeIndex];
//        QString identifier = QString::fromUtf8(compute->id);

//        if(!m_dataMap.contains(identifier)) {
//            anyChanges = true;
//            addCompute(identifier, lammpsController);
//        }
//    }

//    QList<CPCompute*> computesToBeRemoved;
//    for(QObject *object : m_data) {
//        CPCompute *compute = qobject_cast<CPCompute*>(object);
//        if(!lammpsController->computeExists(compute->identifier())) {
//            // Not in LAMMPS anymore, we should remove it too
//            anyChanges = true;
//            computesToBeRemoved.push_back(compute);
//        }
//    }

//    for(CPCompute *compute : computesToBeRemoved) {
//        removeCompute(compute->identifier());
//    }

//    for(QObject *obj : m_data) {
//        CPCompute *compute = qobject_cast<CPCompute*>(obj);
//        for(QVariant &variant : compute->data1D()) {
//            CP1DData *data = variant.value<CP1DData *>();
//            emit data->updated();
//        }
//    }

//    if(anyChanges) {
//        setModel(QVariant::fromValue(m_data));
//        setCount(m_data.size());
//    }

//    if(!lammpsController->state.canProcessSimulatorControls) return;
//    for(QObject *object : m_data) {
//        CPCompute *compute = qobject_cast<CPCompute*>(object);
//        compute->copyData(lammpsController);
//    }
}

int Variables::count() const
{
    return m_count;
}

QVariant Variables::model() const
{
    return m_model;
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

}

void Variables::remove(QString identifier)
{

}

void Variables::reset()
{
//    QList<QObject*> data = m_data;
//    for(QObject *object : data) {
//        CPVariable *variable = qobject_cast<CPCompute*>(object);
//        removeCompute(compute->identifier());
//    }
//    data.clear();
}
