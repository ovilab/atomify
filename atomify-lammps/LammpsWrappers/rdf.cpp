//#include "rdf.h"
//#include "../lammpscontroller.h"
//#include <compute_rdf.h>
//#include <fix_ave_time.h>
//RDF::RDF()
//{

//}

//QString RDF::generatePairString()
//{
//    QString str;
//    for(const QVariant &variant : m_atomPairs) {
//        str.append(QString("%1 ").arg(variant.value<double>()));
//    }
//    return str;
//}

//int RDF::numberOfPairs()
//{
//    return m_atomPairs.size()/2;
//}

//void RDF::updateCommand()
//{
//    setCommand(QString("compute %1 %2 rdf %3 %4").arg(identifier()).arg(m_group).arg(m_numberOfBins).arg(generatePairString()));
//}

//QList<QString> RDF::enabledCommands()
//{
//    return {
//        QString("compute %1 %2 rdf %3 %4").arg(identifier()).arg(m_group).arg(m_numberOfBins).arg(generatePairString()),
//        QString("fix %1_ave all ave/time 1 1 1 c_%1 file %1.txt mode vector").arg(identifier())
//    };
//}

//QList<QString> RDF::disableCommands()
//{
//    return { QString("unfix %1_ave").arg(identifier()),
//             QString("uncompute %1").arg(identifier())
//    };
//}

//QList<QString> RDF::resetCommands()
//{
//    return { QString("unfix %1_ave").arg(identifier()),
//             QString("uncompute %1").arg(identifier()),
//             QString("compute %1 %2 rdf %3 %4").arg(identifier()).arg(m_group).arg(m_numberOfBins).arg(generatePairString()),
//             QString("fix %1_ave all ave/time 1 1 1 c_%1 file %1.txt mode vector ave running").arg(identifier())
//    };
//}

//void RDF::update(LAMMPSController *lammpsController)
//{
//    SimulatorControl::update(lammpsController);
//}

//QVariantList RDF::atomPairs() const
//{
//    return m_atomPairs;
//}

//QString RDF::group() const
//{
//    return m_group;
//}

//int RDF::numberOfBins() const
//{
//    return m_numberOfBins;
//}

//DataSource *RDF::dataSource1() const
//{
//    return m_dataSource1;
//}

//DataSource *RDF::dataSource2() const
//{
//    return m_dataSource2;
//}

//DataSource *RDF::dataSource3() const
//{
//    return m_dataSource3;
//}

//DataSource *RDF::dataSource4() const
//{
//    return m_dataSource4;
//}

//void RDF::setAtomPairs(QVariantList atomPairs)
//{
//    if (m_atomPairs == atomPairs)
//        return;

//    m_atomPairs = atomPairs;

//    emit atomPairsChanged(atomPairs);
//}

//void RDF::setGroup(QString group)
//{
//    if (m_group == group)
//        return;

//    m_group = group;
//    emit groupChanged(group);
//}

//void RDF::setNumberOfBins(int numberOfBins)
//{
//    if (m_numberOfBins == numberOfBins)
//        return;

//    m_numberOfBins = numberOfBins;
//    emit numberOfBinsChanged(numberOfBins);
//}

//void RDF::setDataSource1(DataSource *dataSource1)
//{
//    if (m_dataSource1 == dataSource1)
//        return;

//    m_dataSource1 = dataSource1;
//    emit dataSource1Changed(dataSource1);
//}

//void RDF::setDataSource2(DataSource *dataSource2)
//{
//    if (m_dataSource2 == dataSource2)
//        return;

//    m_dataSource2 = dataSource2;
//    emit dataSource2Changed(dataSource2);
//}

//void RDF::setDataSource3(DataSource *dataSource3)
//{
//    if (m_dataSource3 == dataSource3)
//        return;

//    m_dataSource3 = dataSource3;
//    emit dataSource3Changed(dataSource3);
//}

//void RDF::setDataSource4(DataSource *dataSource4)
//{
//    if (m_dataSource4 == dataSource4)
//        return;

//    m_dataSource4 = dataSource4;
//    emit dataSource4Changed(dataSource4);
//}
