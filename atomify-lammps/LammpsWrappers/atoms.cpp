#include "atoms.h"
#include <atom.h>
#include <domain.h>
#include <neighbor.h>
#include <neigh_list.h>
#include "modifiers/modifiers.h"
#include "mysimulator.h"
#include "cpbonds.h"
#include "system.h"
using namespace LAMMPS_NS;

Atoms::Atoms(AtomifySimulator *simulator)
{
    // Colors from JMOL: http://jmol.sourceforge.net/jscolors/
    // Atomic radii from wikipedia (van der waals radius)

    m_bonds = new CPBonds();
    m_atomData.neighborList.bonds = m_bonds;

    m_atomStyleTypes.insert("hydrogen", new AtomStyle(1.20, "#CCCCCC"));
    m_atomStyleTypes.insert("helium", new AtomStyle(1.40, "#D9FFFF"));
    m_atomStyleTypes.insert("lithium", new AtomStyle(1.82, "#CC80FF"));
    m_atomStyleTypes.insert("beryllium", new AtomStyle(1.53, "#C2FF00"));
    m_atomStyleTypes.insert("boron", new AtomStyle(1.92, "#FFB5B5"));
    m_atomStyleTypes.insert("carbon", new AtomStyle(1.70, "#909090"));
    m_atomStyleTypes.insert("nitrogen", new AtomStyle(1.55, "#3050F8"));
    m_atomStyleTypes.insert("oxygen", new AtomStyle(1.52, "#AA0000"));
    m_atomStyleTypes.insert("fluorine", new AtomStyle(1.35, "#90E050"));
    m_atomStyleTypes.insert("neon", new AtomStyle(1.54, "#3050F8"));
    m_atomStyleTypes.insert("sodium", new AtomStyle(2.27, "#AB5CF2"));
    m_atomStyleTypes.insert("magnesium", new AtomStyle(1.73, "#8AFF00"));
    m_atomStyleTypes.insert("aluminium", new AtomStyle(1.84, "#BFA6A6"));
    m_atomStyleTypes.insert("silicon", new AtomStyle(2.27, "#F0C8A0"));
    m_atomStyleTypes.insert("phosphorus", new AtomStyle(1.80, "#FF8000"));
    m_atomStyleTypes.insert("sulfur", new AtomStyle(1.80, "#FFFF30"));
    m_atomStyleTypes.insert("chlorine", new AtomStyle(1.75, "#1FF01F"));
    m_atomStyleTypes.insert("argon", new AtomStyle(1.88, "#80D1E3"));
    m_atomStyleTypes.insert("potassium", new AtomStyle(2.75, "#8F40D4"));
    m_atomStyleTypes.insert("calcium", new AtomStyle(2.31, "#3DFF00"));


    m_atomStyles.push_back(m_atomStyleTypes["hydrogen"]);
    m_atomStyles.push_back(m_atomStyleTypes["helium"]);
    m_atomStyles.push_back(m_atomStyleTypes["lithium"]);
    m_atomStyles.push_back(m_atomStyleTypes["beryllium"]);
    m_atomStyles.push_back(m_atomStyleTypes["boron"]);
    m_atomStyles.push_back(m_atomStyleTypes["carbon"]);
    m_atomStyles.push_back(m_atomStyleTypes["nitrogen"]);
    m_atomStyles.push_back(m_atomStyleTypes["oxygen"]);
    m_atomStyles.push_back(m_atomStyleTypes["fluorine"]);
    m_atomStyles.push_back(m_atomStyleTypes["neon"]);
    m_atomStyles.push_back(m_atomStyleTypes["sodium"]);
    m_atomStyles.push_back(m_atomStyleTypes["magnesium"]);
    m_atomStyles.push_back(m_atomStyleTypes["aluminium"]);
    m_atomStyles.push_back(m_atomStyleTypes["silicon"]);
    m_atomStyles.push_back(m_atomStyleTypes["phosphorus"]);
    m_atomStyles.push_back(m_atomStyleTypes["sulfur"]);
    m_atomStyles.push_back(m_atomStyleTypes["chlorine"]);
    m_atomStyles.push_back(m_atomStyleTypes["argon"]);
    m_atomStyles.push_back(m_atomStyleTypes["potassium"]);
    m_atomStyles.push_back(m_atomStyleTypes["calcium"]);

}

void Atoms::synchronize(LAMMPS *lammps)
{
    if(!lammps) {
        m_atomData.reset();
        return;
    }
    Atom *atom = lammps->atom;
    Domain *domain = lammps->domain;
    int *types = lammps->atom->type;

    int numberOfAtoms = atom->natoms;
    m_atomData.resize(numberOfAtoms);

    for(QVector3D &delta : m_atomData.deltaPositions) delta = QVector3D(); // Reset delta
    for(QVector3D &color : m_atomData.colors) color = QVector3D(0.9, 0.2, 0.1);
    for(float &radii : m_atomData.radii) radii = 1.0;
    for(bool &visible : m_atomData.visible) visible = true;

    for(int i=0; i<numberOfAtoms; i++) {
        m_atomData.types[i] = types[i];

        double position[3];
        position[0] = atom->x[i][0];
        position[1] = atom->x[i][1];
        position[2] = atom->x[i][2];
        domain->remap(position); // remap into system boundaries with PBC
        m_atomData.positions[i][0] = position[0];
        m_atomData.positions[i][1] = position[1];
        m_atomData.positions[i][2] = position[2];
        m_atomData.originalIndex[i] = i;
    }

    // if(m_bonds->enabled()) m_atomData.neighborList.synchronize(lammps); // Disabled because we don't use it. We now use lammps neighbor list instead
}

void Atoms::updateData(System *system, LAMMPS *lammps)
{
    AtomData atomData = m_atomData;
    if(!atomData.isValid()) {
        qDebug() << "Atom data is not valid before modifiers.";
        exit(1);
    }

    for(QVariant &modifier_ : m_modifiers) {
        Modifier *modifier = modifier_.value<Modifier*>();
        modifier->setSystem(system);
         modifier->apply(atomData);
         if(!atomData.isValid()) {
             // TODO: insert modifier name to debug message
             qDebug() << "Atom data is not valid after modifier.";
             exit(1);
         }
    }

    applyDeltaPositions(atomData);
    generateSphereData(atomData);
    generateBondData(atomData);
}

void Atoms::applyDeltaPositions(AtomData &atomData) {
    for(int i=0; i<atomData.positions.size(); i++) {
        QVector3D &position = atomData.positions[i];
        QVector3D &deltaPosition = atomData.deltaPositions[i];
        position[0] += deltaPosition[0];
        position[1] += deltaPosition[1];
        position[2] += deltaPosition[2];
    }
}

void Atoms::generateSphereData(AtomData &atomData) {
    int visibleAtomCount = 0;
    m_spheresData.resize(atomData.size());
    for(int atomIndex = 0; atomIndex<atomData.size(); atomIndex++) {
        if(atomData.visible[atomIndex]) {
            m_spheresData[visibleAtomCount].position = atomData.positions[atomIndex];
            m_spheresData[visibleAtomCount].color = atomData.colors[atomIndex];
            m_spheresData[visibleAtomCount].scale = atomData.radii[atomIndex];
            visibleAtomCount++;
        }
    }
    qDebug() << "We now have " << visibleAtomCount << " visible atoms";
    m_spheresData.resize(visibleAtomCount);
}

void Atoms::generateBondData(AtomData &atomData) {
//    bondsDataRaw.resize(0);
//    if(!m_bonds->enabled()) {
//        // m_bondData->setData(bondsDataRaw);
//        return;
//    }

//    const Neighborlist &neighborList = atomData.neighborList;
//    if(neighborList.neighbors.size()==0) return;

//    QElapsedTimer t;
//    t.start();
//    bondsDataRaw.reserve(atomData.positions.size());
//    for(int ii=0; ii<atomData.size(); ii++) {
//        int i = atomData.originalIndex[ii];

//        const QVector3D position_i = atomData.positions[ii];
//        const QVector3D deltaPosition_i = atomData.deltaPositions[ii];
//        const int atomType_i = atomData.types[ii];

//        const QVector<float> bondLengths = m_bonds->bondLengths()[atomType_i];
//        const float sphereRadius_i = atomData.radii[ii];

//        if(neighborList.neighbors.size() <= i) continue;

//        for(const int &j : neighborList.neighbors[i]) {
//            QVector3D position_j = atomData.positions[j];
//            position_j[0] += deltaPosition_i[0];
//            position_j[1] += deltaPosition_i[1];
//            position_j[2] += deltaPosition_i[2];

//            const int &atomType_j = atomData.types[j];

//            float dx = position_i[0] - position_j[0];
//            float dy = position_i[1] - position_j[1];
//            float dz = position_i[2] - position_j[2];
//            float rsq = dx*dx + dy*dy + dz*dz; // Componentwise has 10% lower execution time than (position_i - position_j).lengthSquared()
//            if(rsq < bondLengths[atomType_j]*bondLengths[atomType_j] ) {
////                BondVBOData bond;
////                bond.vertex1[0] = position_i[0];
////                bond.vertex1[1] = position_i[1];
////                bond.vertex1[2] = position_i[2];
////                bond.vertex2[0] = position_j[0];
////                bond.vertex2[1] = position_j[1];
////                bond.vertex2[2] = position_j[2];
////                bond.radius1 = m_bondRadius;
////                bond.radius2 = m_bondRadius;
////                bond.sphereRadius1 = sphereRadius_i;
////                bond.sphereRadius2 = atomData.radii[j];
////                bondsDataRaw.push_back(bond);
//            }
//        }
//    }

//    // qDebug() << bondsDataRaw.size() << " bonds created in " << t.elapsed()  << " ms. Memory usage: " << bondsDataRaw.size()*sizeof(BondVBOData);

//    // m_bondData->setData(bondsDataRaw);
}

QVector<AtomStyle *> &Atoms::atomStyles()
{
    return m_atomStyles;
}

void Atoms::setAtomType(int atomType, QString elementName)
{
    if(!m_atomStyleTypes.contains(elementName)) {
        qDebug() << "Warning, element " << elementName << " was not found.";
        return;
    }

    m_atomStyles[atomType] = m_atomStyleTypes[elementName];
}

void Atoms::setAtomColorAndScale(int atomType, QColor color, float radius)
{
    if(atomType >= m_atomStyles.size()) return;

    m_atomStyles[atomType]->color = color;
    m_atomStyles[atomType]->radius = radius;
}

CPBonds *Atoms::bonds() const
{
    return m_bonds;
}

AtomData &Atoms::atomData()
{
    return m_atomData;
}

float Atoms::bondRadius() const
{
    return m_bondRadius;
}

void Atoms::reset()
{
    m_atomData.reset();
}

QVariantList Atoms::modifiers() const
{
    return m_modifiers;
}

void Atoms::setBondRadius(float bondRadius)
{
    if (m_bondRadius == bondRadius)
        return;

    m_bondRadius = bondRadius;
    emit bondRadiusChanged(bondRadius);
}

void Atoms::setModifiers(QVariantList modifiers)
{
    if (m_modifiers == modifiers)
        return;

    m_modifiers = modifiers;
    emit modifiersChanged(modifiers);
}

QVector<BondData> &Atoms::bondsData()
{
    return m_bondsData;
}

QVector<SphereData> &Atoms::spheresData()
{
    return m_spheresData;
}
