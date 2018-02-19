#include "atoms.h"
#include <algorithm>
#include <fix_atomify.h>
#include <atom.h>
#include <domain.h>
#include <neighbor.h>
#include <neigh_request.h>
#include <neigh_list.h>
#include <force.h>
#include <QDir>
#include <QStandardPaths>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include "modifiers/modifiers.h"
#include "mysimulator.h"
#include "bonds.h"
#include "system.h"
using namespace LAMMPS_NS;

Atoms::Atoms(AtomifySimulator *simulator)
{
    readAtomTypesFromFile();
    // Colors from JMOL: http://jmol.sourceforge.net/jscolors/
    // Atomic radii from wikipedia (van der waals radius)
    // https://en.wikipedia.org/wiki/Atomic_radii_of_the_elements_(data_page)
    m_sphereData = new SphereData(simulator);
    m_bondData = new BondData(simulator);
    m_bonds = new Bonds();

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
    if(lammpsController->m_paused) {
        // No new data from LAMMPS when paused
        m_atomData.dirty = true;
        m_atomData.paused = true;
        return;
    }
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
    if(m_atomData.lammpsParticleId.size() != numberOfAtoms) {
        m_atomData.lammpsParticleId.resize(numberOfAtoms);
        for(int &id : m_atomData.lammpsParticleId) id = 0;
    }
    m_atomData.radiiFromLAMMPS = lammps->atom->radius_flag;

    for(int i=0; i<numberOfAtoms; i++) {
        m_atomData.types[i] = types[i];
        m_atomData.originalIndex[i] = i;
        if (lammps->atom->tag_enable) {
            tagint tag = lammps->atom->tag[i];
            m_atomData.lammpsParticleId[i] = tag;
        } else {
            // TODO this might not be the right way to do it, consider giving error message
            m_atomData.lammpsParticleId[i] = i + 1;
        }

        double position[3];
        position[0] = atom->x[i][0];
        position[1] = atom->x[i][1];
        position[2] = atom->x[i][2];
        if(m_atomData.radiiFromLAMMPS) {
            m_atomData.radii[i] = atom->radius[i];
        }
        domain->remap(position); // remap into system boundaries with PBC

        m_atomData.positions[i][0] = position[0]*m_globalScale;
        m_atomData.positions[i][1] = position[1]*m_globalScale;
        m_atomData.positions[i][2] = position[2]*m_globalScale;
        m_atomData.bitmask[i] = atom->mask[i];
        m_atomData.visible[i] = true;
        m_atomData.deltaPositions[i] = QVector3D();
    }
    m_atomData.dirty = true;
    m_atomData.paused = false;
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

float Atoms::globalScale() const
{
    return m_globalScale;
}

void Atoms::setSelectedParticles(QVector<int> particleIds)
{
    m_selectedParticles = particleIds;
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
    if(!dirtyData()) return;

    int numSpheres = m_sphereDataRaw.size() / sizeof(SphereVBOData);
    m_sphereData->setData(m_sphereDataRaw, numSpheres);

    int numBonds = m_bondsDataRaw.size() / sizeof(BondVBOData);
    m_bondData->setData(m_bondsDataRaw, numBonds);
    setDirtyData(false);
}

void Atoms::generateSphereData(AtomData &atomData) {
    if(!atomData.dirty) return;
    int visibleAtomCount = 0;

    float radius = 0.2f * m_bondScale;
    if (m_renderingMode == "Sticks" || m_renderingMode == "Wireframe") {
        radius = 0.1f * m_bondScale;
    }

    for(int i = 0; i<atomData.size(); i++) {
        if(atomData.visible[i]) {
            atomData.positions[visibleAtomCount] = atomData.positions[i];
            atomData.colors[visibleAtomCount] = atomData.colors[i];
            atomData.radii[visibleAtomCount] = atomData.radii[i];
            atomData.deltaPositions[visibleAtomCount] = atomData.deltaPositions[i];
            atomData.radii[visibleAtomCount] = radius;
            atomData.lammpsParticleId[visibleAtomCount] = atomData.lammpsParticleId[i];
            visibleAtomCount++;
        }
    }
    atomData.resize(visibleAtomCount);

    m_sphereDataRaw.resize(visibleAtomCount * sizeof(SphereVBOData));
    SphereVBOData *vboData = reinterpret_cast<SphereVBOData *>(m_sphereDataRaw.data());
    for(int i=0; i<visibleAtomCount; i++) {
        const int particleId = atomData.lammpsParticleId[i];
        SphereVBOData &vbo = vboData[i];
        vbo.position = atomData.positions[i] + atomData.deltaPositions[i];
        vbo.color = atomData.colors[i];
        vbo.radius = atomData.radii[i]*m_sphereScale;
        vbo.particleId = particleId;
        vbo.flags = m_selectedParticles.contains(particleId) ? Selected : 0;
    }
    setDirtyData(true);
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

bool Atoms::generateBondDataFromNeighborList(AtomData &atomData, LAMMPSController *controller)
{

    FixAtomify *fixAtomify = dynamic_cast<FixAtomify*>(controller->findFixByIdentifier("atomify"));
    if(!fixAtomify) {
        qDebug() << "Error, could not find fix_atomify.";
        return false;
    }
    fixAtomify->build_neighborlist = m_bonds->active();
    if(!m_bonds->active()) {
        return false;
    }

    NeighList *list = fixAtomify->list;
    const int inum = list->inum;
    int *numneigh = list->numneigh;
    int **firstneigh = list->firstneigh;

    bondsDataRaw.reserve(atomData.positions.size());
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
            if(rsq < bondLengths[atomType_j]*bondLengths[atomType_j]*m_globalScale*m_globalScale ) {
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

            if(dr2 > dr2max*m_globalScale*m_globalScale || dx > 0.5*controller->system->size().x() || dy > 0.5*controller->system->size().y() || dz > 0.5*controller->system->size().z() ) {
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
    if(!atomData.dirty) return;
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
    setDirtyData(true);
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

void Atoms::setGlobalScale(float globalScale)
{
    if (m_globalScale == globalScale)
        return;

    m_globalScale = globalScale;
    emit globalScaleChanged(globalScale);
}

void Atoms::readAtomTypesFromFile()
{
    QString dataDirPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QString atomDataFileName = QDir(dataDirPath).filePath("extras/atomtypes.json");
    QFile loadFile(atomDataFileName);
    if(!loadFile.open(QIODevice::ReadOnly)) {
        qDebug() << "Could not open atom types file " << atomDataFileName;
        exit(1);
    }
    QByteArray jsonData = loadFile.readAll();
    QJsonParseError error;
    QJsonDocument json(QJsonDocument::fromJson(jsonData, &error));
    if(json.isNull()) {
        qDebug() << "Error in atom types file: " << error.errorString();
        qDebug() << "Delete " << atomDataFileName << " to regenerate the default file.";
        exit(1);
    }
    QJsonArray array = json.array();
    for(int i=0; i<array.size(); i++) {
        QJsonObject atomType = array[i].toObject();
        QString color = atomType["color"].toString();
        QString shortName = atomType["shortname"].toString();
        QString fullName = atomType["fullname"].toString();
        float radius = atomType["radius"].toDouble();
        m_atomStyleTypes.insert(fullName, new AtomStyle(shortName, fullName, radius, color));
    }
}
