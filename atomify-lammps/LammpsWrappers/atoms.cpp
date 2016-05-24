#include "atoms.h"
#include <atom.h>
#include <domain.h>
#include <neighbor.h>
#include <neigh_list.h>
#include "modifiers/modifiers.h"
#include "mysimulator.h"
#include "bonds.h"
using namespace LAMMPS_NS;

Atoms::Atoms(AtomifySimulator *simulator)
{
    // Colors from JMOL: http://jmol.sourceforge.net/jscolors/
    // Atomic radii from wikipedia (van der waals radius)

    m_sphereData = new SphereData(simulator);
    m_bondData = new BondData(simulator);
    m_bonds = new Bonds();
    m_atomData.neighborList.bonds = m_bonds;

    m_atomStyleTypes.insert("hydrogen", new AtomStyle(1.20, "#FFFFFF"));
    m_atomStyleTypes.insert("helium", new AtomStyle(1.40, "#D9FFFF"));
    m_atomStyleTypes.insert("lithium", new AtomStyle(1.82, "#CC80FF"));
    m_atomStyleTypes.insert("beryllium", new AtomStyle(1.53, "#C2FF00"));
    m_atomStyleTypes.insert("boron", new AtomStyle(1.92, "#FFB5B5"));
    m_atomStyleTypes.insert("carbon", new AtomStyle(1.70, "#909090"));
    m_atomStyleTypes.insert("nitrogen", new AtomStyle(1.55, "#3050F8"));
    m_atomStyleTypes.insert("oxygen", new AtomStyle(1.52, "#FF0D0D"));
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
    if(!lammps) return;
    Atom *atom = lammps->atom;
    Domain *domain = lammps->domain;
    int *types = lammps->atom->type;

    int numberOfAtoms = atom->natoms;
    if(m_atomData.positions.size() != numberOfAtoms) {
        m_atomData.positions.resize(numberOfAtoms);
    }
    if(m_atomData.types.size() != numberOfAtoms) {
        m_atomData.types.resize(numberOfAtoms);
    }
    if(m_atomData.colors.size() != numberOfAtoms) {
        m_atomData.colors.resize(numberOfAtoms);
        for(QVector3D &color : m_atomData.colors) color = QVector3D(0.9, 0.2, 0.1);
    }
    if(m_atomData.radii.size() != numberOfAtoms) {
        m_atomData.radii.resize(numberOfAtoms);
        for(float &radii : m_atomData.radii) radii = 1.0;
    }

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
    }

    if(m_bonds->enabled()) m_atomData.neighborList.synchronize(lammps);
}

void Atoms::updateData()
{
    AtomData atomData = m_atomData;
    if(!atomData.isValid()) {
        qDebug() << "Atom data is not valid before modifiers.";
        exit(1);
    }

    for(Modifier *modifier : m_modifiers) {
         modifier->apply(atomData);
         if(!atomData.isValid()) {
             // TODO: insert modifier name to debug message
             qDebug() << "Atom data is not valid after modifier.";
             exit(1);
         }
    }

    generateSphereData(atomData);
    generateBondData(atomData);
}

void Atoms::generateSphereData(AtomData &atomData) {
    m_sphereData->setData(atomData.positions, atomData.colors, atomData.radii);
}

void Atoms::generateBondData(AtomData &atomData) {
    QVector<BondVBOData> bonds;
    if(!m_bonds->enabled()) {
        m_bondData->setData(bonds);
        return;
    }

    const Neighborlist &neighborList = atomData.neighborList;
    if(neighborList.neighbors.size()==0) return;

//    QElapsedTimer t;
//    t.start();
    bonds.reserve(atomData.positions.size()*5);

    for(int i=0; i<atomData.positions.size(); i++) {
        const QVector3D &position_i = atomData.positions[i];
        const int &atomType_i = atomData.types[i];

        for(const int &j : neighborList.neighbors[i]) {
            const QVector3D &position_j = atomData.positions[j];
            const int &atomType_j = atomData.types[j];

            float dx = position_i[0] - position_j[0];
            float dy = position_i[1] - position_j[1];
            float dz = position_i[2] - position_j[2];
            float rsq = dx*dx + dy*dy + dz*dz; // Componentwise has 10% lower execution time than (position_i - position_j).lengthSquared()

            if(rsq < m_bonds->bondLengths()[atomType_i][atomType_j]*m_bonds->bondLengths()[atomType_i][atomType_j] ) {
                BondVBOData bond;
                bond.vertex1 = position_i;
                bond.vertex2 = position_j;
                bond.radius1 = 0.10;
                bond.radius2 = 0.10;
                bond.sphereRadius1 = atomData.radii[i];
                bond.sphereRadius2 = atomData.radii[j];
                bonds.push_back(bond);
            }
        }
    }

    // qDebug() << bonds.size() << " bonds created in " << t.elapsed()  << " ms";

    m_bondData->setData(bonds);
}

SphereData *Atoms::sphereData() const
{
    return m_sphereData;
}

QVector<Modifier *> &Atoms::modifiers()
{
    return m_modifiers;
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

BondData *Atoms::bondData() const
{
    return m_bondData;
}

Bonds *Atoms::bonds() const
{
    return m_bonds;
}

AtomData &Atoms::atomData()
{
    return m_atomData;
}
