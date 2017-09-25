#include "fixes.h"
#include "../lammpscontroller.h"
#include "simulatorcontrols/cpfix.h"

Fixes::Fixes(QObject *parent) : QObject(parent)
{

}

void Fixes::add(QString identifier, LAMMPSController *lammpsController) {
    if(lammpsController->simulatorControls.contains(identifier)) {
        CPFix *fix =  qobject_cast<CPFix*>(lammpsController->simulatorControls[identifier]);
        m_data.push_back(fix);
        m_dataMap.insert(identifier, fix);
    } else {
        CPFix *fix = new CPFix();
        fix->setIsMirror(true);
        fix->setIdentifier(identifier);
        m_data.push_back(fix);
        m_dataMap.insert(identifier, fix);
    }
}

void Fixes::remove(QString identifier) {
    CPFix *fix = qobject_cast<CPFix*>(m_dataMap[identifier]);
    if(fix->isMirror()) {
        m_data.removeOne(fix);
        m_dataMap.remove(identifier);
        delete fix;
    } else {
        m_data.removeOne(fix);
        m_dataMap.remove(identifier);
    }
}

void Fixes::reset() {
    QList<QObject*> data = m_data;
    for(QObject *object : data) {
        CPFix *fix = qobject_cast<CPFix*>(object);
        remove(fix->identifier());
    }
    data.clear();
    setModel(QVariant::fromValue(m_data));
    setCount(0);
}

bool Fixes::addOrRemove(LAMMPSController *lammpsController)
{
    LAMMPS_NS::Modify *modify = lammpsController->lammps()->modify;
    bool anyChanges = false;
    for(int fixIndex=0; fixIndex<modify->nfix; fixIndex++) {

        LAMMPS_NS::Fix *fix = modify->fix[fixIndex];
        QString identifier = QString::fromUtf8(fix->id);

        if(!m_dataMap.contains(identifier)) {
            anyChanges = true;
            add(identifier, lammpsController);
        }
    }

    QList<CPFix*> fixesToBeRemoved;
    for(QObject *object : m_data) {
        CPFix *fix = qobject_cast<CPFix*>(object);
        if(!lammpsController->fixExists(fix->identifier())) {
            // Not in LAMMPS anymore, we should remove it too
            anyChanges = true;
            fixesToBeRemoved.push_back(fix);
        }
    }

    for(CPFix *fix : fixesToBeRemoved) {
        remove(fix->identifier());
    }
    return anyChanges;
}

void Fixes::synchronizeQML(LAMMPSController *lammpsController)
{
    if(!lammpsController->lammps()) return;
    bool anyChanges = addOrRemove(lammpsController);
    if(anyChanges) {
        setModel(QVariant::fromValue(m_data));
        setCount(m_data.count());
    }

}

void Fixes::synchronize(LAMMPSController *lammpsController)
{
    if(!lammpsController->lammps()) return;


    for(QObject *object : m_data) {
        CPFix *fix = qobject_cast<CPFix*>(object);
        fix->copyData(lammpsController);
    }
}

void Fixes::updateThreadOnDataObjects(QThread *thread) {
    for(QObject *obj : m_data) {
        CPFix *fix = qobject_cast<CPFix*>(obj);
        for(QVariant &variant : fix->data1D()) {
            Data1D *data = variant.value<Data1D *>();
            if(data->thread() != thread) {
                data->moveToThread(thread);
            }
        }
    }
}

int Fixes::count() const
{
    return m_count;
}

QVariant Fixes::model() const
{
    return m_model;
}

void Fixes::setModel(QVariant model)
{
    if (m_model == model)
        return;

    m_model = model;
    emit modelChanged(model);
}

void Fixes::setCount(int count)
{
    if (m_count == count)
        return;

    m_count = count;
    emit countChanged(count);
}
