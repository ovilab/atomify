#include "atoms.h"
#include <atom.h>
#include <domain.h>
#include <neighbor.h>
#include <force.h>
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

    m_atomStyleTypes.insert("hydrogen", new AtomStyle(1.20, "#FFFFFF"));
    m_atomStyleTypes.insert("helium", new AtomStyle(1.40, "#D9FFFF"));
    m_atomStyleTypes.insert("lithium", new AtomStyle(1.82, "#CC80FF"));
    m_atomStyleTypes.insert("beryllium", new AtomStyle(1.53, "#C2FF00"));
    m_atomStyleTypes.insert("boron", new AtomStyle(1.92, "#FFB5B5"));
    m_atomStyleTypes.insert("carbon", new AtomStyle(1.70, "#909090"));
    m_atomStyleTypes.insert("nitrogen", new AtomStyle(1.55, "#3050F8"));
    m_atomStyleTypes.insert("oxygen", new AtomStyle(1.52, "#FF0D0D"));
    m_atomStyleTypes.insert("fluorine", new AtomStyle(1.47, "#90E050"));
    m_atomStyleTypes.insert("neon", new AtomStyle(1.54, "#B3E3F5"));
    m_atomStyleTypes.insert("sodium", new AtomStyle(2.27, "#AB5CF2"));
    m_atomStyleTypes.insert("magnesium", new AtomStyle(1.73, "#8AFF00"));
    m_atomStyleTypes.insert("aluminium", new AtomStyle(1.84, "#BFA6A6"));
    m_atomStyleTypes.insert("silicon", new AtomStyle(2.10, "#F0C8A0"));
    m_atomStyleTypes.insert("phosphorus", new AtomStyle(1.80, "#FF8000"));
    m_atomStyleTypes.insert("sulfur", new AtomStyle(1.80, "#FFFF30"));
    m_atomStyleTypes.insert("chlorine", new AtomStyle(1.75, "#1FF01F"));
    m_atomStyleTypes.insert("argon", new AtomStyle(1.88, "#80D1E3"));
    m_atomStyleTypes.insert("potassium", new AtomStyle(2.75, "#8F40D4"));
    m_atomStyleTypes.insert("calcium", new AtomStyle(2.31, "#3DFF00"));
    m_atomStyleTypes.insert("scandium", new AtomStyle(2.11, "#E6E6E6"));
    m_atomStyleTypes.insert("titanium", new AtomStyle(2.00, "#BFC2C7"));
    m_atomStyleTypes.insert("vanadium", new AtomStyle(2.00, "#FF1493"));
    m_atomStyleTypes.insert("chromium", new AtomStyle(2.00, "#8A99C7"));
    m_atomStyleTypes.insert("manganese", new AtomStyle(2.00, "#9C7AC7"));
    m_atomStyleTypes.insert("iron", new AtomStyle(2.00, "#E06633"));
    m_atomStyleTypes.insert("cobalt", new AtomStyle(2.00, "#F090A0"));
    m_atomStyleTypes.insert("nickel", new AtomStyle(1.63, "#50D050"));
    m_atomStyleTypes.insert("copper", new AtomStyle(1.40, "#C88033"));
    m_atomStyleTypes.insert("zinc", new AtomStyle(1.39, "#7D80B0"));
    m_atomStyleTypes.insert("gallium", new AtomStyle(1.87, "#C28F8F"));
    m_atomStyleTypes.insert("germanium", new AtomStyle(2.11, "#668F8F"));
    m_atomStyleTypes.insert("arsenic", new AtomStyle(1.85, "#BD80E3"));
    m_atomStyleTypes.insert("selenium", new AtomStyle(1.90, "#FFA100"));
    m_atomStyleTypes.insert("bromine", new AtomStyle(1.85, "#A62929"));
    m_atomStyleTypes.insert("krypton", new AtomStyle(2.02, "#5CB8D1"));
    m_atomStyleTypes.insert("rubidium", new AtomStyle(3.03, "#702EB0"));
    m_atomStyleTypes.insert("strontium", new AtomStyle(2.49, "#00FF00"));
    m_atomStyleTypes.insert("yttrium", new AtomStyle(2.00, "#94FFFF"));
    m_atomStyleTypes.insert("zirconium", new AtomStyle(2.00, "#94E0E0"));
    m_atomStyleTypes.insert("niobium", new AtomStyle(2.00, "#73C2C9"));
    m_atomStyleTypes.insert("molybdenum", new AtomStyle(2.00, "#54B5B5"));
    m_atomStyleTypes.insert("technetium", new AtomStyle(2.00, "#3B9E9E"));
    m_atomStyleTypes.insert("ruthenium", new AtomStyle(2.00, "#248F8F"));
    m_atomStyleTypes.insert("rhodium", new AtomStyle(2.00, "#0A7D8C"));
    m_atomStyleTypes.insert("palladium", new AtomStyle(1.63, "#006985"));
    m_atomStyleTypes.insert("silver", new AtomStyle(1.72, "#C0C0C0"));
    m_atomStyleTypes.insert("cadmium", new AtomStyle(1.58, "#FFD98F"));
    m_atomStyleTypes.insert("indium", new AtomStyle(1.93, "#A67573"));
    m_atomStyleTypes.insert("tin", new AtomStyle(2.17, "#668080"));
    m_atomStyleTypes.insert("antimony", new AtomStyle(2.06, "#9E63B5"));
    m_atomStyleTypes.insert("tellurium", new AtomStyle(2.06, "#D47A00"));
    m_atomStyleTypes.insert("iodine", new AtomStyle(1.98, "#940094"));
    m_atomStyleTypes.insert("xenon", new AtomStyle(2.16, "#429EB0"));
    m_atomStyleTypes.insert("caesium", new AtomStyle(3.43, "#57178F"));
    m_atomStyleTypes.insert("barium", new AtomStyle(2.68, "#00C900"));
    m_atomStyleTypes.insert("lanthanum", new AtomStyle(2.00, "#70D4FF"));
    m_atomStyleTypes.insert("cerium", new AtomStyle(2.00, "#FFFFC7"));
    m_atomStyleTypes.insert("praseodymium", new AtomStyle(2.00, "#D9FFC7"));
    m_atomStyleTypes.insert("neodymium", new AtomStyle(2.00, "#C7FFC7"));
    m_atomStyleTypes.insert("promethium", new AtomStyle(2.00, "#A3FFC7"));
    m_atomStyleTypes.insert("samarium", new AtomStyle(2.00, "#8FFFC7"));
    m_atomStyleTypes.insert("europium", new AtomStyle(2.00, "#61FFC7"));
    m_atomStyleTypes.insert("gadolinium", new AtomStyle(2.00, "#45FFC7"));
    m_atomStyleTypes.insert("terbium", new AtomStyle(2.00, "#30FFC7"));
    m_atomStyleTypes.insert("dysprosium", new AtomStyle(2.00, "#1FFFC7"));
    m_atomStyleTypes.insert("holmium", new AtomStyle(2.00, "#00FF9C"));
    m_atomStyleTypes.insert("erbium", new AtomStyle(2.00, "#00E675"));
    m_atomStyleTypes.insert("thulium", new AtomStyle(2.00, "#00D452"));
    m_atomStyleTypes.insert("ytterbium", new AtomStyle(2.00, "#00BF38"));
    m_atomStyleTypes.insert("lutetium", new AtomStyle(2.00, "#00AB24"));
    m_atomStyleTypes.insert("hafnium", new AtomStyle(2.00, "#4DC2FF"));
    m_atomStyleTypes.insert("tantalum", new AtomStyle(2.00, "#4DA6FF"));
    m_atomStyleTypes.insert("tungsten", new AtomStyle(2.00, "#2194D6"));
    m_atomStyleTypes.insert("rhenium", new AtomStyle(2.00, "#267DAB"));
    m_atomStyleTypes.insert("osmium", new AtomStyle(2.00, "#266696"));
    m_atomStyleTypes.insert("iridium", new AtomStyle(2.00, "#175487"));
    m_atomStyleTypes.insert("platinum", new AtomStyle(1.75, "#D0D0E0"));
    m_atomStyleTypes.insert("gold", new AtomStyle(1.66, "#FFD123"));
    m_atomStyleTypes.insert("mercury", new AtomStyle(1.55, "#B8B8D0"));
    m_atomStyleTypes.insert("thallium", new AtomStyle(1.96, "#A6544D"));
    m_atomStyleTypes.insert("lead", new AtomStyle(2.02, "#575961"));
    m_atomStyleTypes.insert("bismuth", new AtomStyle(2.07, "#9E4FB5"));
    m_atomStyleTypes.insert("polonium", new AtomStyle(1.97, "#AB5C00"));
    m_atomStyleTypes.insert("astatine", new AtomStyle(2.02, "#754F45"));
    m_atomStyleTypes.insert("radon", new AtomStyle(2.20, "#428296"));
    m_atomStyleTypes.insert("francium", new AtomStyle(3.48, "#420066"));
    m_atomStyleTypes.insert("radium", new AtomStyle(2.83, "#007D00"));
    m_atomStyleTypes.insert("actinium", new AtomStyle(2.00, "#70ABFA"));
    m_atomStyleTypes.insert("thorium", new AtomStyle(2.00, "#00BAFF"));
    m_atomStyleTypes.insert("protactinium", new AtomStyle(2.00, "#00A1FF"));
    m_atomStyleTypes.insert("uranium", new AtomStyle(1.86, "#008FFF"));
    m_atomStyleTypes.insert("neptunium", new AtomStyle(2.00, "#0080FF"));
    m_atomStyleTypes.insert("plutonium", new AtomStyle(2.00, "#006BFF"));
    m_atomStyleTypes.insert("americium", new AtomStyle(2.00, "#545CF2"));
    m_atomStyleTypes.insert("curium", new AtomStyle(2.00, "#785CE3"));
    m_atomStyleTypes.insert("berkelium", new AtomStyle(2.00, "#8A4FE3"));
    m_atomStyleTypes.insert("californium", new AtomStyle(2.00, "#A136D4"));
    m_atomStyleTypes.insert("einsteinium", new AtomStyle(2.00, "#B31FD4"));
    m_atomStyleTypes.insert("fermium", new AtomStyle(2.00, "#B31FBA"));
    m_atomStyleTypes.insert("mendelevium", new AtomStyle(2.00, "#B30DA6"));
    m_atomStyleTypes.insert("nobelium", new AtomStyle(2.00, "#BD0D87"));
    m_atomStyleTypes.insert("lawrencium", new AtomStyle(2.00, "#C70066"));

    for(int i=0; i<50; i++) {
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

    if(m_atomData.colors.size() != numberOfAtoms) {
        m_atomData.colors.resize(numberOfAtoms);
    }
    if(m_atomData.deltaPositions.size() != numberOfAtoms) {
        m_atomData.deltaPositions.resize(numberOfAtoms);
    }
    if(m_atomData.occlusion.size() != numberOfAtoms) {
        m_atomData.occlusion.resize(numberOfAtoms);
    }
    if(m_atomData.originalIndex.size() != numberOfAtoms) {
        m_atomData.originalIndex.resize(numberOfAtoms);
    }

    if(m_atomData.positions.size() != numberOfAtoms) {
        m_atomData.positions.resize(numberOfAtoms);
    }

    if(m_atomData.radii.size() != numberOfAtoms) {
        m_atomData.radii.resize(numberOfAtoms);
    }
    if(m_atomData.types.size() != numberOfAtoms) {
        m_atomData.types.resize(numberOfAtoms);
    }
    if(m_atomData.visible.size() != numberOfAtoms) {
        m_atomData.visible.resize(numberOfAtoms);
    }

    for(QVector3D &delta : m_atomData.deltaPositions) delta = QVector3D(); // Reset delta
    for(QVector3D &color : m_atomData.colors) color = QVector3D(0.9, 0.2, 0.1);
    for(float &radius : m_atomData.radii) radius = m_sphereRadius;
    for(bool &visible : m_atomData.visible) visible = true;

    for(int i=0; i<numberOfAtoms; i++) {
        m_atomData.types[i] = types[i];

        double position[3];
        position[0] = atom->x[i][0];
        position[1] = atom->x[i][1];
        position[2] = atom->x[i][2];
        domain->remap(position); // remap into system boundaries with PBC
        m_atomData.occlusion[i] = 0.0;
        m_atomData.positions[i][0] = position[0] - (lammps->domain->boxlo[0] + lammps->domain->prd_half[0]);
        m_atomData.positions[i][1] = position[1] - (lammps->domain->boxlo[1] + lammps->domain->prd_half[1]);
        m_atomData.positions[i][2] = position[2] - (lammps->domain->boxlo[2] + lammps->domain->prd_half[2]);
        m_atomData.originalIndex[i] = i;
    }

    if(m_bonds->enabled()) m_atomData.neighborList.synchronize(lammps); // Disabled because we don't use it. We now use lammps neighbor list instead
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
    generateBondData(atomData, lammps);
    generateSphereData(atomData);
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
            m_spheresData[visibleAtomCount].occlusion = atomData.occlusion[atomIndex] / m_occlusionFactor;
            m_spheresData[visibleAtomCount].color = atomData.colors[atomIndex];
            m_spheresData[visibleAtomCount].scale = atomData.radii[atomIndex];
            visibleAtomCount++;
        }
    }
    m_spheresData.resize(visibleAtomCount);
}

bool Atoms::doWeHavefullNeighborList(Neighbor *neighbor) {
    // We will find the first atom i with at least one non-ghost neighbor. We then check if atom i is in the list of j's neighbors
    NeighList *list = neighbor->lists[0];
    const int inum = list->inum;
    int *numneigh = list->numneigh;
    int **firstneigh = list->firstneigh;
    int *ilist = list->ilist;

    for (int ii = 0; ii < inum; ii++) {
        int i = ilist[ii];
        int *jlist = firstneigh[i];
        int jnum = numneigh[i];
        for (int jj = 0; jj < jnum; jj++) {
            int j = jlist[jj];
            j &= NEIGHMASK;
            if(j<inum) {
                // We found an atom i with non-ghost neighbor j
                int *otherlist = firstneigh[j];
                int othernum = numneigh[j];
                for (int kk = 0; kk < othernum; kk++) {
                    // If any of its neighbors is atom i, we have full list
                    int k = otherlist[kk];
                    k &= NEIGHMASK;
                    if(k==i) return true;
                }
                // If none of its neighbors is atom i, half list
                return false;
            }
        }
    }
}

bool Atoms::generateBondDataFromNeighborList(AtomData &atomData, LAMMPSController *controller)
{
//    if(!m_bonds->enabled()) {
//        return false;
//    }

//    bool hasNeighborLists = controller->lammps()->neighbor->nlist > 0;
//    if(!hasNeighborLists) {
//        return false;
//    }

//    Neighbor *neighbor = controller->lammps()->neighbor;
//    NeighList *list = neighbor->lists[0];
//    const int inum = list->inum;
//    int *numneigh = list->numneigh;
//    int **firstneigh = list->firstneigh;

//    bool fullNeighborList = doWeHavefullNeighborList(neighbor);
//    for(int ii=0; ii<atomData.size(); ii++) {
//        if(!atomData.visible[ii]) continue;
//        int i = atomData.originalIndex[ii];

//        const QVector3D deltaPosition_i = atomData.deltaPositions[ii];
//        const QVector3D position_i = atomData.positions[ii] + deltaPosition_i;
//        const int atomType_i = atomData.types[ii];
//        if(m_bonds->bondLengths().size()<=atomType_i) continue; // We only store atom bonds between atom types < 32

//        const QVector<float> &bondLengths = m_bonds->bondLengths()[atomType_i];
//        const float sphereRadius_i = atomData.radii[ii];

//        if(i >= inum) continue; // Atom i is outside the neighbor list. Will probably not happen, but let's skip in that case.

//        int *jlist = firstneigh[i];
//        int jnum = numneigh[i];
//        for (int jj = 0; jj < jnum; jj++) {
//            int j = jlist[jj];
//            j &= NEIGHMASK;
//            if(fullNeighborList && j<i) continue; // Newton's third law applies on full lists. We don't draw 2 bonds per pair.
//            if(j >= atomData.size()) continue; // Probably a ghost atom from LAMMPS
//            if(!atomData.visible[j]) continue; // Don't show bond if not both atoms are visible.

//            const int &atomType_j = atomData.types[j];
//            if(bondLengths.size()<=atomType_j) continue; // We only store atom bonds between atom types < 32

//            QVector3D position_j = atomData.positions[j];
//            position_j += deltaPosition_i;

//            float dx = position_i[0] - position_j[0];
//            float dy = position_i[1] - position_j[1];
//            float dz = position_i[2] - position_j[2];
//            float rsq = dx*dx + dy*dy + dz*dz; // Componentwise has 10% lower execution time than (position_i - position_j).lengthSquared()
//            if(rsq < bondLengths[atomType_j]*bondLengths[atomType_j] ) {
//                BondVBOData bond;
//                bond.vertex1[0] = position_i[0];
//                bond.vertex1[1] = position_i[1];
//                bond.vertex1[2] = position_i[2];
//                bond.vertex2[0] = position_j[0];
//                bond.vertex2[1] = position_j[1];
//                bond.vertex2[2] = position_j[2];
//                float bondRadius = 0.1*m_bondScale; // TODO: move this magic number to a variable
//                bond.radius1 = bondRadius;
//                bond.radius2 = bondRadius;
//                bond.sphereRadius1 = sphereRadius_i*m_sphereScale;
//                bond.sphereRadius2 = atomData.radii[j]*m_sphereScale;
//                bondsDataRaw.push_back(bond);
//            }
//        }
//    }
//    return true;
}

bool Atoms::generateBondDataFromBondList(AtomData &atomData, LAMMPS *lammps)
{
    Atom *atom = lammps->atom;
    if(atom->nbonds==0) return false;
    for(int ii=0; ii<atomData.size(); ii++) {
        int i = atomData.originalIndex[ii];
        QVector3D position_i = atomData.positions[ii];
        const QVector3D deltaPosition = atomData.deltaPositions[ii];
        position_i += deltaPosition;

        for(int jj=0; jj<atom->num_bond[i]; jj++) {
            int j = atom->map(atom->bond_atom[i][jj]);
            if(j < 0 || j>=atomData.size()) continue;
            if(!lammps->force->newton_bond && i<j) continue;

            QVector3D position_j = atomData.positions[j] + deltaPosition;
            float dx = fabs(position_i[0] - position_j[0]);
            float dy = fabs(position_i[1] - position_j[1]);
            float dz = fabs(position_i[2] - position_j[2]);
            double dr2 = dx*dx + dy*dy + dz*dz;
            double dr2max = 40; // arbitrary units. TODO!

            // if(dr2 > dr2max || dx > 0.5*controller->system->size().x() || dy > 0.5*controller->system->size().y() || dz > 0.5*controller->system->size().z() ) {
            if(dr2 > dr2max) {
                // Periodic image
                continue;
            }

            BondData bond;
            bond.vertex1Position[0] = position_i[0];
            bond.vertex1Position[1] = position_i[1];
            bond.vertex1Position[2] = position_i[2];

            bond.vertex2Position[0] = position_j[0];
            bond.vertex2Position[1] = position_j[1];
            bond.vertex2Position[2] = position_j[2];

            bond.radius1 = m_bondRadius;
            bond.radius2 = m_bondRadius;

            bond.sphereRadius1 = atomData.radii[ii];
            bond.sphereRadius2 = atomData.radii[j];
            m_bondsData.push_back(bond);
        }
    }

    return true;
}

void Atoms::generateBondData(AtomData &atomData, LAMMPS *lammps) {
    m_bondsData.resize(0);

    generateBondDataFromBondList(atomData, lammps);
//    if(generateBondDataFromBondList(atomData, lammps)) {
//        return;
//    }

    if(!m_bonds->enabled()) {
        return;
    }

    const Neighborlist &neighborList = atomData.neighborList;
    if(neighborList.neighbors.size()==0) return;

    QElapsedTimer t;
    t.start();
    m_bondsData.reserve(atomData.positions.size()+m_bondsData.size());
    for(int ii=0; ii<atomData.size(); ii++) {
        int i = atomData.originalIndex[ii];

        const QVector3D position_i = atomData.positions[ii];
        const QVector3D deltaPosition_i = atomData.deltaPositions[ii];
        const int atomType_i = atomData.types[ii];

        const QVector<float> &bondLengths = m_bonds->bondLengths()[atomType_i];
        if(i >= neighborList.neighbors.size()) continue;

        for(const int &j : neighborList.neighbors[i]) {
            // if(j<ii) continue; // TODO: fix so we get this "newton's 3rd law" on bonds if neighbor lists are full
            QVector3D position_j = atomData.positions[j];
            position_j[0] += deltaPosition_i[0];
            position_j[1] += deltaPosition_i[1];
            position_j[2] += deltaPosition_i[2];

            const int &atomType_j = atomData.types[j];

            float dx = position_i[0] - position_j[0];
            float dy = position_i[1] - position_j[1];
            float dz = position_i[2] - position_j[2];
            float rsq = dx*dx + dy*dy + dz*dz; // Componentwise has 10% lower execution time than (position_i - position_j).lengthSquared()
            if(rsq < 10) {
                atomData.occlusion[ii] += 1;
            }
            if(rsq < bondLengths[atomType_j]*bondLengths[atomType_j] ) {
                BondData bond;
                bond.vertex1Position[0] = position_i[0];
                bond.vertex1Position[1] = position_i[1];
                bond.vertex1Position[2] = position_i[2];

                bond.vertex2Position[0] = position_j[0];
                bond.vertex2Position[1] = position_j[1];
                bond.vertex2Position[2] = position_j[2];

                bond.radius1 = m_bondRadius;
                bond.radius2 = m_bondRadius;

                bond.sphereRadius1 = atomData.radii[ii];
                bond.sphereRadius2 = atomData.radii[j];
                m_bondsData.push_back(bond);
            }
        }
    }
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
    m_spheresData.clear();
    m_bondsData.clear();
    m_atomData.reset();
    m_bonds->reset();
    m_atomStyles.clear();
    for(int i=0; i<50; i++) {
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

void Atoms::setSphereRadius(float sphereRadius)
{
    if (m_sphereRadius == sphereRadius)
            return;

        m_sphereRadius = sphereRadius;
        emit bondSphereChanged(sphereRadius);
}

void Atoms::setOcclusionFactor(float occlusionFactor)
{
    if (m_occlusionFactor == occlusionFactor)
            return;

        m_occlusionFactor = occlusionFactor;
        emit occlusionFactorChanged(occlusionFactor);
}

QVector<BondData> &Atoms::bondsData()
{
    return m_bondsData;
}

float Atoms::sphereRadius() const
{
    return m_sphereRadius;
}

float Atoms::occlusionFactor() const
{
    return m_occlusionFactor;
}

long Atoms::memoryUsage()
{
    return 2*m_atomData.memoryUsage() + m_bondsData.capacity()*sizeof(BondData) + m_spheresData.capacity()*sizeof(SphereData);
}

QVector<SphereData> &Atoms::spheresData()
{
    return m_spheresData;
}
