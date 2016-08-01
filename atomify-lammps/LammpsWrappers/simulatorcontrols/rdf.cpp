#include "rdf.h"
#include "../../lammpscontroller.h"
#include <compute_rdf.h>
#include <fix_ave_time.h>
RDF::RDF(Qt3DCore::QNode *parent) : CPCompute(parent)
{

}

QString RDF::generatePairString()
{
    QString str;
    for(const QVariant &variant : m_atomPairs) {
        str.append(QString("%1 ").arg(variant.value<double>()));
    }
    return str;
}

int RDF::numberOfPairs()
{
    return m_atomPairs.size()/2;
}

void RDF::updateCommand()
{
    setCommand(QString("rdf %2 %3").arg(numberOfBins()).arg(generatePairString()));
}

QList<QString> RDF::enabledCommands()
{
    updateCommand();
    return {
        fullCommand(),
        QString("fix %1_ave all ave/time 1 1 1 c_%1 file %1.txt mode vector ave running").arg(identifier())
    };
}

QList<QString> RDF::disableCommands()
{
    return { QString("unfix %1_ave").arg(identifier()),
             QString("uncompute %1").arg(identifier())
    };
}

QList<QString> RDF::resetCommands()
{
    return { QString("unfix %1_ave").arg(identifier()),
             QString("uncompute %1").arg(identifier()),
             fullCommand(),
             QString("fix %1_ave all ave/time 1 1 1 c_%1 file %1.txt mode vector ave running").arg(identifier())
    };
}

void RDF::update(LAMMPSController *lammpsController)
{
//    SimulatorControl::update(lammpsController);
//    QString fixAveTimeIdentifier = QString("%1_ave").arg(identifier());
//    LAMMPS_NS::FixAveTime *lmp_fix = dynamic_cast<LAMMPS_NS::FixAveTime*>(lammpsController->findFixByIdentifier(fixAveTimeIdentifier));
//    if(lmp_fix != nullptr) {
//        if(dataSource())  dataSource()->clear();
//        if(dataSource1()) dataSource1()->clear();
//        if(dataSource2()) dataSource2()->clear();
//        if(dataSource3()) dataSource3()->clear();
//        if(dataSource4()) dataSource4()->clear();

//        for(int i=0; i<numberOfBins(); i++) {
//            float x = lmp_fix->compute_array(i,0);
//            if(numberOfPairs() >= 1 && dataSource()) {
//                float y = lmp_fix->compute_array(i,1);
//                dataSource()->addPoint(x,y);
//            }
//            if(numberOfPairs() >= 2 && dataSource1()) {
//                float y = lmp_fix->compute_array(i,3);
//                dataSource1()->addPoint(x,y);
//            }
//            if(numberOfPairs() >= 3 && dataSource2()) {
//                float y = lmp_fix->compute_array(i,5);
//                dataSource2()->addPoint(x,y);
//            }
//            if(numberOfPairs() >= 4 && dataSource3()) {
//                float y = lmp_fix->compute_array(i,7);
//                dataSource3()->addPoint(x,y);
//            }
//            if(numberOfPairs() >= 5 && dataSource4()) {
//                float y = lmp_fix->compute_array(i,9);
//                dataSource4()->addPoint(x,y);
//            }
//        }
//        if(dataSource())  dataSource()->update();
//        if(dataSource1()) dataSource1()->update();
//        if(dataSource2()) dataSource2()->update();
//        if(dataSource3()) dataSource3()->update();
//        if(dataSource4()) dataSource4()->update();

//    }
}

QVariantList RDF::atomPairs() const
{
    return m_atomPairs;
}

int RDF::numberOfBins() const
{
    return m_numberOfBins;
}

DataSource *RDF::dataSource1() const
{
    return m_dataSource1;
}

DataSource *RDF::dataSource2() const
{
    return m_dataSource2;
}

DataSource *RDF::dataSource3() const
{
    return m_dataSource3;
}

DataSource *RDF::dataSource4() const
{
    return m_dataSource4;
}

void RDF::setAtomPairs(QVariantList atomPairs)
{
    if (m_atomPairs == atomPairs)
        return;

    m_atomPairs = atomPairs;

    emit atomPairsChanged(atomPairs);
}

void RDF::setNumberOfBins(int numberOfBins)
{
    if (m_numberOfBins == numberOfBins)
        return;

    m_numberOfBins = numberOfBins;
    emit numberOfBinsChanged(numberOfBins);
}

void RDF::setDataSource1(DataSource *dataSource1)
{
    if (m_dataSource1 == dataSource1)
        return;

    m_dataSource1 = dataSource1;
    emit dataSource1Changed(dataSource1);
}

void RDF::setDataSource2(DataSource *dataSource2)
{
    if (m_dataSource2 == dataSource2)
        return;

    m_dataSource2 = dataSource2;
    emit dataSource2Changed(dataSource2);
}

void RDF::setDataSource3(DataSource *dataSource3)
{
    if (m_dataSource3 == dataSource3)
        return;

    m_dataSource3 = dataSource3;
    emit dataSource3Changed(dataSource3);
}

void RDF::setDataSource4(DataSource *dataSource4)
{
    if (m_dataSource4 == dataSource4)
        return;

    m_dataSource4 = dataSource4;
    emit dataSource4Changed(dataSource4);
}
