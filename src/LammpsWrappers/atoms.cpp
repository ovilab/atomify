#include "atoms.h"
#include <algorithm>
#include <atom.h>
#include <domain.h>
#include <neighbor.h>
#include <neigh_request.h>
#include <neigh_list.h>
#include <force.h>
#include "modifiers/modifiers.h"
#include "mysimulator.h"
#include "bonds.h"
#include "system.h"
using namespace LAMMPS_NS;

Atoms::Atoms(AtomifySimulator *simulator)
{
    // Colors from JMOL: http://jmol.sourceforge.net/jscolors/
    // Atomic radii from wikipedia (van der waals radius)
    // https://en.wikipedia.org/wiki/Atomic_radii_of_the_elements_(data_page)
    m_sphereData = new SphereData(simulator);
    m_bondData = new BondData(simulator);
    m_bonds = new Bonds();

    m_atomStyleTypes.insert("hydrogen", new AtomStyle(1.20, "#CCCCCC"));
    m_atomStyleTypes.insert("helium", new AtomStyle(1.40, "#D9FFFF"));
    m_atomStyleTypes.insert("lithium", new AtomStyle(1.82, "#CC80FF"));
    m_atomStyleTypes.insert("beryllium", new AtomStyle(1.53, "#C2FF00"));
    m_atomStyleTypes.insert("boron", new AtomStyle(1.92, "#FFB5B5"));
    m_atomStyleTypes.insert("carbon", new AtomStyle(1.70, "#505050"));
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
    m_atomStyleTypes.insert("scandium", new AtomStyle(2.11, "#E6E6E6"));
    m_atomStyleTypes.insert("titanium", new AtomStyle(2.00, "#BFC2C7"));
    m_atomStyleTypes.insert("vanadium", new AtomStyle(2.00, "#A6A6AB"));
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

    for(int i=0; i<200; i++) {
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

void Atoms::synchronize(LAMMPSController *lammpsController)
{
    LAMMPS *lammps = lammpsController->lammps();
    if(!lammps) { return; }

    Atom *atom = lammps->atom;
    Domain *domain = lammps->domain;
    int *types = lammps->atom->type;
    int numberOfAtoms = atom->natoms;

    if(m_atomData.positions.size() != numberOfAtoms) {
        m_atomData.positions.resize(numberOfAtoms);
    }
    if(m_atomData.deltaPositions.size() != numberOfAtoms) {
        m_atomData.deltaPositions.resize(numberOfAtoms);
        for(QVector3D &delta : m_atomData.deltaPositions) delta = QVector3D(); // Reset delta
    }
    if(m_atomData.types.size() != numberOfAtoms) {
        m_atomData.types.resize(numberOfAtoms);
    }
    if(m_atomData.bitmask.size() != numberOfAtoms) {
        m_atomData.bitmask.resize(numberOfAtoms);
    }
    if(m_atomData.visible.size() != numberOfAtoms) {
        m_atomData.visible.resize(numberOfAtoms);
    }
    if(m_atomData.colors.size() != numberOfAtoms) {
        m_atomData.colors.resize(numberOfAtoms);
        for(QVector3D &color : m_atomData.colors) color = QVector3D(0.9, 0.2, 0.1);
    }
    if(m_atomData.originalIndex.size() != numberOfAtoms) {
        m_atomData.originalIndex.resize(numberOfAtoms);
    }
    if(m_atomData.radii.size() != numberOfAtoms) {
        m_atomData.radii.resize(numberOfAtoms);
        for(float &radii : m_atomData.radii) radii = 1.0;
    }

    // lammps->
    m_atomData.radiiFromLAMMPS = lammps->atom->radius_flag;
    for(int i=0; i<numberOfAtoms; i++) {
        m_atomData.types[i] = types[i];
        m_atomData.originalIndex[i] = i;

        double position[3];
        position[0] = atom->x[i][0];
        position[1] = atom->x[i][1];
        position[2] = atom->x[i][2];
        if(m_atomData.radiiFromLAMMPS) {
            m_atomData.radii[i] = atom->radius[i];
        }
        domain->remap(position); // remap into system boundaries with PBC

        m_atomData.positions[i][0] = position[0];
        m_atomData.positions[i][1] = position[1];
        m_atomData.positions[i][2] = position[2];
        m_atomData.bitmask[i] = atom->mask[i];
        m_atomData.visible[i] = true;
        m_atomData.deltaPositions[i] = QVector3D();
    }

    // if(m_bonds->enabled()) m_atomData.neighborList.synchronize(lammps);
}

void Atoms::processModifiers(System *system)
{
    m_atomDataProcessed = m_atomData;
    if(!m_atomDataProcessed.isValid()) {
        qDebug() << "Atom data is not valid before modifiers.";
        exit(1);
    }

    for(QVariant &modifier_ : m_modifiers) {
        Modifier *modifier = modifier_.value<Modifier*>();
        modifier->setSystem(system);
        modifier->apply(m_atomDataProcessed);
        if(!m_atomDataProcessed.isValid()) {
            // TODO: insert modifier name to debug message
            qDebug() << "Atom data is not valid after modifier.";
            exit(1);
        }
    }
}

void Atoms::createRenderererData(LAMMPSController *lammpsController) {
    generateSphereData(m_atomDataProcessed);
    generateBondData(m_atomDataProcessed, lammpsController);
}

long Atoms::memoryUsage()
{
    return m_atomData.memoryUsage() + m_atomDataProcessed.memoryUsage() +
           (m_bondsDataRaw.capacity() + m_sphereDataRaw.capacity())*sizeof(char) +
           bondsDataRaw.size()*sizeof(BondVBOData);
}

float Atoms::bondScale() const
{
    return m_bondScale;
}

float Atoms::sphereScale() const
{
    return m_sphereScale;
}

QString Atoms::renderingMode() const
{
    return m_renderingMode;
}

void Atoms::synchronizeRenderer() {
    int numSpheres = m_sphereDataRaw.size() / sizeof(SphereVBOData);
    m_sphereData->setData(m_sphereDataRaw, numSpheres);

    int numBonds = m_bondsDataRaw.size() / sizeof(BondVBOData);
    m_bondData->setData(m_bondsDataRaw, numBonds);
}
void Atoms::generateSphereData(AtomData &atomData) {
    int visibleAtomCount = 0;

    for(int i = 0; i<atomData.size(); i++) {
        if(atomData.visible[i]) {
            atomData.positions[visibleAtomCount] = atomData.positions[i];
            atomData.colors[visibleAtomCount] = atomData.colors[i];
            atomData.radii[visibleAtomCount] = atomData.radii[i];
            if(m_renderingMode == "Stick") atomData.radii[visibleAtomCount] = 0.1*m_bondScale;
            else if(m_renderingMode == "Wireframe") atomData.radii[visibleAtomCount] = 0.1*m_bondScale;
            visibleAtomCount++;
        }
    }
    atomData.positions.resize(visibleAtomCount);
    atomData.colors.resize(visibleAtomCount);
    atomData.radii.resize(visibleAtomCount);

    m_sphereDataRaw.resize(visibleAtomCount * sizeof(SphereVBOData));
    SphereVBOData *vboData = reinterpret_cast<SphereVBOData *>(m_sphereDataRaw.data());
    for(int i=0; i<visibleAtomCount; i++) {
        SphereVBOData &vbo = vboData[i];
        vbo.position = atomData.positions[i] + atomData.deltaPositions[i];
        vbo.color = atomData.colors[i];
        vbo.radius = atomData.radii[i]*m_sphereScale;
    }
}

bool Atoms::dirtyData() const
{
    return m_dirtyData;
}

void Atoms::setDirtyData(bool dirtyData)
{
    m_dirtyData = dirtyData;
}

int Atoms::numberOfBonds() const
{
    return m_numberOfBonds;
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
    if(!m_bonds->active()) {
        return false;
    }

    bool hasNeighborLists = controller->lammps()->neighbor->nlist > 0;
    if(!hasNeighborLists) {
        return false;
    }

    Neighbor *neighbor = controller->lammps()->neighbor;
    NeighList *list = neighbor->lists[0];
    const int inum = list->inum;
    int *numneigh = list->numneigh;
    int **firstneigh = list->firstneigh;

    bondsDataRaw.reserve(atomData.positions.size());
    bool fullNeighborList = doWeHavefullNeighborList(neighbor);
    for(int ii=0; ii<atomData.size(); ii++) {
        if(!atomData.visible[ii]) continue;
        int i = atomData.originalIndex[ii];

        const QVector3D deltaPosition_i = atomData.deltaPositions[ii];
        const QVector3D position_i = atomData.positions[ii] + deltaPosition_i;
        const int atomType_i = atomData.types[ii];
        if(m_bonds->bondLengths().size()<=atomType_i) continue; // We only store atom bonds between atom types < 32

        const QVector<float> &bondLengths = m_bonds->bondLengths()[atomType_i];
        const float sphereRadius_i = atomData.radii[ii];

        if(i >= inum) continue; // Atom i is outside the neighbor list. Will probably not happen, but let's skip in that case.

        int *jlist = firstneigh[i];
        int jnum = numneigh[i];
        for (int jj = 0; jj < jnum; jj++) {
            int j = jlist[jj];
            j &= NEIGHMASK;
            if(fullNeighborList && j<i) continue; // Newton's third law applies on full lists. We don't draw 2 bonds per pair.
            if(j >= atomData.size()) continue; // Probably a ghost atom from LAMMPS
            if(!atomData.visible[j]) continue; // Don't show bond if not both atoms are visible.

            const int &atomType_j = atomData.types[j];
            if(bondLengths.size()<=atomType_j) continue; // We only store atom bonds between atom types < 32

            QVector3D position_j = atomData.positions[j];
            position_j += deltaPosition_i;

            float dx = position_i[0] - position_j[0];
            float dy = position_i[1] - position_j[1];
            float dz = position_i[2] - position_j[2];
            float rsq = dx*dx + dy*dy + dz*dz; // Componentwise has 10% lower execution time than (position_i - position_j).lengthSquared()
            if(rsq < bondLengths[atomType_j]*bondLengths[atomType_j] ) {
                BondVBOData bond;
                bond.vertex1[0] = position_i[0];
                bond.vertex1[1] = position_i[1];
                bond.vertex1[2] = position_i[2];
                bond.vertex2[0] = position_j[0];
                bond.vertex2[1] = position_j[1];
                bond.vertex2[2] = position_j[2];
                float bondRadius = 0.1*m_bondScale; // TODO: move this magic number to a variable
                bond.radius1 = bondRadius;
                bond.radius2 = bondRadius;
                bond.sphereRadius1 = sphereRadius_i*m_sphereScale;
                bond.sphereRadius2 = atomData.radii[j]*m_sphereScale;
                bondsDataRaw.push_back(bond);
            }
        }
    }
    return true;
}

bool Atoms::generateBondDataFromBondList(AtomData &atomData, LAMMPSController *controller)
{
    Atom *atom = controller->lammps()->atom;
    if(atom->nbonds==0) return false;
    for(int ii=0; ii<atomData.size(); ii++) {
        int i = atomData.originalIndex[ii];
        QVector3D position_i = atomData.positions[ii];
        const QVector3D deltaPosition = atomData.deltaPositions[ii];
        position_i += deltaPosition;

        for(int jj=0; jj<atom->num_bond[i]; jj++) {
            int j = atom->map(atom->bond_atom[i][jj]);
            if(j < 0 || j>=atomData.size()) continue;
            if(!controller->lammps()->force->newton_bond && i<j) continue;

            QVector3D position_j = atomData.positions[j] + deltaPosition;
            float dx = fabs(position_i[0] - position_j[0]);
            float dy = fabs(position_i[1] - position_j[1]);
            float dz = fabs(position_i[2] - position_j[2]);
            double dr2 = dx*dx + dy*dy + dz*dz;
            double dr2max = 40; // arbitrary units. TODO!

            if(dr2 > dr2max || dx > 0.5*controller->system->size().x() || dy > 0.5*controller->system->size().y() || dz > 0.5*controller->system->size().z() ) {
                // Periodic image
                continue;
            }

            BondVBOData bond;
            bond.vertex1[0] = position_i[0];
            bond.vertex1[1] = position_i[1];
            bond.vertex1[2] = position_i[2];
            bond.vertex2[0] = position_j[0];
            bond.vertex2[1] = position_j[1];
            bond.vertex2[2] = position_j[2];
            float bondRadius = 0.1*m_bondScale;
            bond.radius1 = bondRadius;
            bond.radius2 = bondRadius;
            bond.sphereRadius1 = atomData.radii[i]*m_sphereScale;
            bond.sphereRadius2 = atomData.radii[j]*m_sphereScale;
            bondsDataRaw.push_back(bond);
        }
    }

    return true;
}

void Atoms::generateBondData(AtomData &atomData, LAMMPSController *controller) {
    bondsDataRaw.resize(0);

    bool didCreateFromNeighborList = generateBondDataFromNeighborList(atomData, controller);
    bool didCreateFromBondList = generateBondDataFromBondList(atomData, controller);

    if(!didCreateFromBondList && !didCreateFromNeighborList) {
        m_bondsDataRaw.clear();
        return;
    }

    setNumberOfBonds(bondsDataRaw.size());
    m_bondsDataRaw.resize(bondsDataRaw.size() * sizeof(BondVBOData));
    BondVBOData *posData = reinterpret_cast<BondVBOData*>(m_bondsDataRaw.data());
    // TODO can we just set the address here? Instead of copying.
    for(const BondVBOData &pos : bondsDataRaw) {
        *posData++ = pos;
    }
}

SphereData *Atoms::sphereData() const
{
    return m_sphereData;
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

void Atoms::setAtomColorAndScale(int atomType, QColor color, float size)
{
    if(atomType >= m_atomStyles.size()) return;

    m_atomStyles[atomType]->color = color;
    m_atomStyles[atomType]->radius = size;
}

void Atoms::setAtomSize(int atomType, float radius) {
    if(atomType >= m_atomStyles.size()) return;

    m_atomStyles[atomType]->radius = radius;
}

void Atoms::setAtomColor(int atomType, QColor color) {
    if(atomType >= m_atomStyles.size()) return;

    m_atomStyles[atomType]->color = color;
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

void Atoms::reset()
{
    QVector<SphereVBOData> emptySphereVBOData;
    QVector<BondVBOData> emptyBondVBOData;
    m_sphereData->setData(emptySphereVBOData);
    m_bondData->setData(emptyBondVBOData);
    m_atomData.reset();
    m_atomDataProcessed.reset();
    m_bonds->reset();
    m_atomStyles.clear();
    m_bondsDataRaw.clear();
    m_sphereDataRaw.clear();
    setNumberOfBonds(0);

    m_atomStyleTypes["helium"]->radius = 1.40;          m_atomStyleTypes["helium"]->color = QColor("#D9FFFF");
    m_atomStyleTypes["lithium"]->radius = 1.82;         m_atomStyleTypes["lithium"]->color = QColor("#CC80FF");
    m_atomStyleTypes["beryllium"]->radius = 1.53;       m_atomStyleTypes["beryllium"]->color = QColor("#C2FF00");
    m_atomStyleTypes["boron"]->radius = 1.92;           m_atomStyleTypes["boron"]->color = QColor("#FFB5B5");
    m_atomStyleTypes["carbon"]->radius = 1.70;          m_atomStyleTypes["carbon"]->color = QColor("#505050");
    m_atomStyleTypes["nitrogen"]->radius = 1.55;        m_atomStyleTypes["nitrogen"]->color = QColor("#3050F8");
    m_atomStyleTypes["oxygen"]->radius = 1.52;          m_atomStyleTypes["oxygen"]->color = QColor("#AA0000");
    m_atomStyleTypes["fluorine"]->radius = 1.35;        m_atomStyleTypes["fluorine"]->color = QColor("#90E050");
    m_atomStyleTypes["neon"]->radius = 1.54;            m_atomStyleTypes["neon"]->color = QColor("#3050F8");
    m_atomStyleTypes["sodium"]->radius = 2.27;          m_atomStyleTypes["sodium"]->color = QColor("#AB5CF2");
    m_atomStyleTypes["magnesium"]->radius = 1.73;       m_atomStyleTypes["magnesium"]->color = QColor("#8AFF00");
    m_atomStyleTypes["aluminium"]->radius = 1.84;       m_atomStyleTypes["aluminium"]->color = QColor("#BFA6A6");
    m_atomStyleTypes["silicon"]->radius = 2.27;         m_atomStyleTypes["silicon"]->color = QColor("#F0C8A0");
    m_atomStyleTypes["phosphorus"]->radius = 1.80;      m_atomStyleTypes["phosphorus"]->color = QColor("#FF8000");
    m_atomStyleTypes["sulfur"]->radius = 1.80;          m_atomStyleTypes["sulfur"]->color = QColor("#FFFF30");
    m_atomStyleTypes["chlorine"]->radius = 1.75;        m_atomStyleTypes["chlorine"]->color = QColor("#1FF01F");
    m_atomStyleTypes["argon"]->radius = 1.88;           m_atomStyleTypes["argon"]->color = QColor("#80D1E3");
    m_atomStyleTypes["potassium"]->radius = 2.75;       m_atomStyleTypes["potassium"]->color = QColor("#8F40D4");
    m_atomStyleTypes["calcium"]->radius = 2.31;         m_atomStyleTypes["calcium"]->color = QColor("#3DFF00");

    for(int i=0; i<200; i++) {
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

bool Atoms::sort() const
{
    return m_sort;
}

QVariantList Atoms::modifiers() const
{
    return m_modifiers;
}

void Atoms::setModifiers(QVariantList modifiers)
{
    if (m_modifiers == modifiers)
        return;

    m_modifiers = modifiers;
    emit modifiersChanged(modifiers);
}

void Atoms::setSort(bool sort)
{
    if (m_sort == sort)
        return;

    m_sort = sort;
    emit sortChanged(sort);
}

void Atoms::setBondScale(float bondScale)
{
    if (m_bondScale == bondScale)
        return;

    m_bondScale = bondScale;
    emit bondScaleChanged(bondScale);
}

void Atoms::setSphereScale(float sphereScale)
{
    if (m_sphereScale == sphereScale)
        return;

    m_sphereScale = sphereScale;
    emit sphereScaleChanged(sphereScale);
}

void Atoms::setRenderingMode(QString renderingMode)
{
    if (m_renderingMode == renderingMode)
        return;

    m_renderingMode = renderingMode;
    emit renderingModeChanged(renderingMode);
}

void Atoms::setNumberOfBonds(int numberOfBonds)
{
    if (m_numberOfBonds == numberOfBonds)
        return;

    m_numberOfBonds = numberOfBonds;
    emit numberOfBondsChanged(numberOfBonds);
}
