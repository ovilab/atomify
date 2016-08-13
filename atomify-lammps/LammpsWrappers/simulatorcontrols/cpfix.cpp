#include "cpfix.h"
#include "lammpscontroller.h"
#include "../system.h"
#include "../../dataproviders/data2d.h"
#include <style_compute.h>
#include <iostream>
using namespace std;

CPFix::CPFix(Qt3DCore::QNode *parent) : SimulatorControl(parent)
{
}

CPFix::~CPFix() { }

bool CPFix::copyData(FixAveChunk *fix, LAMMPSController *lammpsController) {
    enum{BIN1D,BIN2D,BIN3D,BINSPHERE,BINCYLINDER,
         TYPE,MOLECULE,COMPUTE,FIX,VARIABLE};

    if(!fix) return false;
    if(fix->cchunk->which == BIN2D) {
        if(m_dataRaw.size() != fix->nvalues) {
            m_dataRaw.clear();
            m_data.clear();
            for(int i=0; i<fix->nvalues; i++) {
                Data2D *data = new Data2D(this);
                m_dataRaw.push_back(data);
                m_data.push_back(QVariant::fromValue(data));
            }
        }

        if(fix->nextvalid() == lammpsController->system()->timesteps()+1) {
            float xMin = fix->cchunk->origin[0];
            float zMin = fix->cchunk->origin[1];
            float deltaX = fix->cchunk->delta[0];
            float deltaZ = fix->cchunk->delta[1];
            float xMax = xMin + deltaX;
            float zMax = zMin + deltaZ;
            QSize size(fix->cchunk->nlayers[0], fix->cchunk->nlayers[1]);
            xMin = lammpsController->system()->origin().x();
            xMax = lammpsController->system()->origin().x() + lammpsController->system()->size().x();
            zMin = lammpsController->system()->origin().y();
            zMax = lammpsController->system()->origin().y() + lammpsController->system()->size().y();

            for(int i=0; i<fix->nvalues; i++) {
                m_dataRaw[i]->setSize(size);
                m_dataRaw[i]->setXMin(xMin);
                m_dataRaw[i]->setXMax(xMax);
                m_dataRaw[i]->setYMin(zMin);
                m_dataRaw[i]->setYMax(zMax);
                m_dataRaw[i]->setZMin(zMin);
                m_dataRaw[i]->setZMax(zMax);
                m_dataRaw[i]->plane = Data2D::XY;
            }

             for(int i=0; i<fix->nchunk; i++) {
                 float x = fix->compute_array(i,0);
                 float y = fix->compute_array(i,1);

                 for(int j=0; j<fix->nvalues; j++) {
                     int valueIndex = fix->colextra+1+j;
                     float z = fix->compute_array(i,valueIndex);
                     m_dataRaw[j]->setValue(x,y,z);
                 }
             }

             for(int i=0; i<fix->nvalues; i++) {
                 m_dataRaw[i]->update();
             }
        }
    }
}

void CPFix::copyData(LAMMPSController *lammpsController)
{
    // if(lammpsController->system()->timesteps() % m_frequency != 0) return;
    if(lastUpdate != -1 && (lammpsController->system()->timesteps()-lastUpdate) < m_frequency) return;
    // if(lammpsController->system()->timesteps() % m_frequency != 0) return;
    Fix *lmp_fix = lammpsController->findFixByIdentifier(identifier());
    if(lmp_fix == nullptr) return;
    if(copyData(dynamic_cast<FixAveChunk*>(lmp_fix), lammpsController)) return;
}

bool CPFix::interactive() const
{
    return m_interactive;
}

QVariantList CPFix::data() const
{
    return m_data;
}

void CPFix::update(LAMMPSController *lammpsController)
{
    SimulatorControl::update(lammpsController);
}

QList<QString> CPFix::enabledCommands()
{
    return { fullCommand() };
}

QList<QString> CPFix::disableCommands()
{
    return {QString("unfix %1 ").arg(identifier())};
}

void CPFix::updateCommand()
{

}

bool CPFix::existsInLammps(LAMMPSController *lammpsController)
{
    return lammpsController->fixExists(identifier());
}

QString CPFix::group() const
{
    return m_group;
}

int CPFix::frequency() const
{
    return m_frequency;
}

void CPFix::setGroup(QString group)
{
    if (m_group == group)
        return;

    m_group = group;
    emit groupChanged(group);
}

void CPFix::setFrequency(int frequency)
{
    if (m_frequency == frequency)
        return;

    m_frequency = frequency;
    emit frequencyChanged(frequency);
}

void CPFix::setInteractive(bool interactive)
{
    if (m_interactive == interactive)
        return;

    m_interactive = interactive;
    emit interactiveChanged(interactive);
}

void CPFix::setData(QVariantList data)
{
    if (m_data == data)
        return;

    m_data = data;
    emit dataChanged(data);
}

QList<QString> CPFix::resetCommands()
{
    return { QString("unfix %1").arg(identifier()), fullCommand() };
}

QString CPFix::createCommandPrefix()
{
    return QString("fix %1 %2 ").arg(identifier()).arg(group());
}
