#include "cpcompute.h"
#include "lammpscontroller.h"
#include "mysimulator.h"
#include "../system.h"
#include <QDebug>
#include <error.h>
#include <update.h>
#include <atom.h>
using std::vector;

CPCompute::CPCompute(Qt3DCore::QNode *parent) : SimulatorControl(parent)
{

}

CPCompute::~CPCompute() { }

void CPCompute::clear()
{
    for(Data1D *data : m_data1DRaw) {
        data->clear();
        emit data->updated();
    }
}

void CPCompute::save(QString filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return;
    }
}

Data1D *CPCompute::ensureExists(QString key, bool enabledByDefault) {
    if(!m_data1DRaw.contains(key)) {
        Data1D *data = new Data1D();
        data->setEnabled(enabledByDefault);
        m_data1DRaw.insert(key, data);
        m_data1D.insert(key, QVariant::fromValue<Data1D*>(data));
    }
    return m_data1DRaw[key];
}

int CPCompute::groupBit() const
{
    return m_groupBit;
}

const vector<double> &CPCompute::atomData() const
{
    return m_atomData;
}

int CPCompute::perAtomIndex() const
{
    return m_perAtomIndex;
}

int CPCompute::numPerAtomValues() const
{
    return m_numPerAtomValues;
}

bool CPCompute::hovered() const
{
    return m_hovered;
}

bool CPCompute::copyData(Compute *compute, LAMMPSController *lammpsController) {
    if(!compute) return false;
    if(!compute->peratom_flag) return false;
    // if(compute->size_peratom_cols > 0) return false;
    int numAtoms = lammpsController->system->numberOfAtoms();
    int numCols = compute->size_peratom_cols;

    if(numCols == 0) {
        setNumPerAtomValues(1);
        double *values = compute->vector_atom;
        m_atomData = vector<double>(values, values+numAtoms);
    } else {
        double **values = compute->array_atom;
        setNumPerAtomValues(numCols);
        m_atomData.resize(numAtoms);
        for(int atomIndex=0; atomIndex<numAtoms; atomIndex++) {
            int atomGroupBit = lammpsController->lammps()->atom->mask[atomIndex];
            if(atomGroupBit & m_groupBit) {
                m_atomData[atomIndex] = values[atomIndex][m_perAtomIndex];
            } else {
                m_atomData[atomIndex] = std::numeric_limits<double>::quiet_NaN();
            }
        }
    }
    Data1D *data = ensureExists("histogram", true);
    data->createHistogram(m_atomData);

    setIsPerAtom(true);
    setInteractive(true);
    return true;
}

bool CPCompute::copyData(ComputeKEAtom *compute, LAMMPSController *lammpsController) {
    return false;
//    if(!compute) return false;
//    ensureExists("histogram", true);
//    double *values = compute->vector_atom;
//    int numAtoms = lammpsController->system->numberOfAtoms();
//    m_atomData = vector<double>(values, values+numAtoms);

//    Data1D *data = ensureExists("histogram", true);
//    data->createHistogram(m_atomData);

//    setIsPerAtom(true);
//    setInteractive(true);
//    return true;
}

bool CPCompute::copyData(ComputeClusterAtom *compute, LAMMPSController *lammpsController) {
    return false;
//    if(!compute) return false;

//    ensureExists("histogram", true);
//    double *values = compute->vector_atom;
//    int numAtoms = lammpsController->system->numberOfAtoms();
//    m_atomData = vector<double>(values, values+numAtoms);

//    Data1D *data = ensureExists("histogram", true);
//    data->createHistogram(m_atomData);

//    setIsPerAtom(true);
//    setInteractive(true);
//    return true;
}

bool CPCompute::copyData(ComputeCNAAtom *compute, LAMMPSController *lammpsController) {
    return false;
//    if(!compute) return false;

//    ensureExists("histogram", true);
//    double *values = compute->vector_atom;
//    int numAtoms = lammpsController->system->numberOfAtoms();
//    m_atomData = vector<double>(values, values+numAtoms);

//    Data1D *data = ensureExists("histogram", true);
//    data->createHistogram(m_atomData);

//    setIsPerAtom(true);
//    setInteractive(true);
//    return true;
}

bool CPCompute::copyData(ComputeTemp *compute, LAMMPSController *lammpsController) {
    if(!compute) return false;
    double value = compute->scalar;
    setHasScalarData(true);
    setScalarValue(value);
    Data1D *data = ensureExists(QString("Temperature"), true);
    setXLabel("Time");
    setYLabel("Temperature");
    qDebug() << " Copying temp data with value: " << value << " and time: " << lammpsController->system->simulationTime();
    data->add(lammpsController->system->simulationTime(), value, true);
    setInteractive(true);
    return true;
}

bool CPCompute::copyData(ComputePropertyAtom *compute, LAMMPSController *lammpsController) {
    return false;

//    if(!compute) return false;
//    if(compute->size_peratom_cols > 0) return true; // We don't support vector quantities yet

//    double *values = compute->vector_atom;
//    int numAtoms = lammpsController->system->numberOfAtoms();
//    m_atomData = vector<double>(values, values+numAtoms);

//    Data1D *data = ensureExists("histogram", true);
//    data->createHistogram(m_atomData);

//    setIsPerAtom(true);
//    setInteractive(true);
//    return true;
}

bool CPCompute::copyData(ComputePE *compute, LAMMPSController *lammpsController) {
    if(!compute) return false;
    if(lammpsController->lammps()->update->ntimestep != lammpsController->lammps()->update->eflag_global) return true;
    double value = compute->scalar;
    setHasScalarData(true);
    setScalarValue(value);
    Data1D *data = ensureExists(QString("Potential energy"), true);
    setXLabel("Time");
    setYLabel("Potential Energy");
    data->add(lammpsController->system->simulationTime(), value);
    setInteractive(true);
    return true;
}

bool CPCompute::copyData(ComputeKE *compute, LAMMPSController *lammpsController) {
    if(!compute) return false;
    double value = compute->scalar;
    setHasScalarData(true);
    setScalarValue(value);
    Data1D *data = ensureExists(QString("Kinetic energy"), true);
    setXLabel("Time");
    setYLabel("Kinetic Energy");
    data->add(lammpsController->system->simulationTime(), value);
    setInteractive(true);
    return true;
}

bool CPCompute::copyData(ComputePressure *compute, LAMMPSController *lammpsController) {
    if(!compute) return false;
    bool virialComputed = lammpsController->lammps()->update->ntimestep == lammpsController->lammps()->update->vflag_global;
    if(!virialComputed) return true;

    // First compute scalar pressure
    double value = compute->scalar;
    setHasScalarData(true);
    setScalarValue(value);
    Data1D *data = ensureExists(QString("Pressure"), true);
    setXLabel("Time");
    setYLabel("Pressure");
    data->add(lammpsController->system->simulationTime(), value);

    // Then compute stress tensor
    // compute->compute_vector();
    // xx, yy, zz, xy, xz, yz
    QStringList components = {"xx", "yy", "zz", "xy", "xz", "yz"};

    int numVectorValues = 6;
    for(int i=1; i<=numVectorValues; i++) {
        QString key = components[i-1];
        Data1D *data = ensureExists(key, false);
        double value = compute->vector[i-1];
        data->add(lammpsController->system->simulationTime(), value);
    }
    setInteractive(true);
    return true;
}

bool CPCompute::copyData(ComputeRDF *compute, LAMMPSController *lammpsController) {
    Q_UNUSED(lammpsController);
    if(!compute) return false;
    // compute->compute_array();
    int numBins = compute->size_array_rows;         // rows in global array
    int numColumns = compute->size_array_cols;      // columns in global array
    int numPairs = (numColumns - 1)/2;

    for(int pairId=0; pairId<numPairs; pairId++) {
        QString key = QString("Pair %1").arg(pairId+1);
        Data1D *data = ensureExists(key, true);
        data->clear();

        for(int bin=0; bin<numBins; bin++) {
            double r = compute->array[bin][0];
            double rdf = compute->array[bin][1+2*pairId];
            data->add(r,rdf,true);
        }

        emit data->updated();
    }
    setInteractive(true);
    return true;
}

bool CPCompute::copyData(ComputeMSD *compute, LAMMPSController *lammpsController) {
    if(!compute) return false;
    // compute->compute_vector();

    // http://www.ascii.cl/htmlcodes.htm
    QStringList components = {"∆x<sup>2</sup>", "∆y<sup>2</sup>", "∆z<sup>2</sup>", "∆r<sup>2</sup>"};

    int numVectorValues = 4;
    for(int i=1; i<=numVectorValues; i++) {
        QString key = components[i-1];
        Data1D *data = ensureExists(key, false);
        double value = compute->vector[i-1];
        data->add(lammpsController->system->simulationTime(), value);
    }
    setInteractive(true);
    return true;
}

bool CPCompute::copyData(ComputeVACF *compute, LAMMPSController *lammpsController) {
    if(!compute) return false;
    // compute->compute_vector();

    // &lt; because < is used as a html tag. Using HTML names instead: http://www.ascii.cl/htmlcodes.htm
    QStringList components = {"&lt;vx, vx0\&gt;", "&lt;vy, vy0&gt;", "&lt;vz, vz0&gt;", "&lt;v, v0&gt;"};

    int numVectorValues = 4;
    for(int i=1; i<=numVectorValues; i++) {
        QString key = components[i-1];
        Data1D *data = ensureExists(key, false);
        double value = compute->vector[i-1];
        data->add(lammpsController->system->simulationTime(), value);
    }
    setInteractive(true);
    return true;
}

bool CPCompute::copyData(ComputeCOM *compute, LAMMPSController *lammpsController) {
    if(!compute) return false;
    // compute->compute_vector();

    // &lt; because < is used as a html tag. Using HTML names instead: http://www.ascii.cl/htmlcodes.htm
    QStringList components = {"x_cm", "y_cm", "z_cm"};

    int numVectorValues = 3;
    for(int i=1; i<=numVectorValues; i++) {
        QString key = components[i-1];
        Data1D *data = ensureExists(key, false);
        double value = compute->vector[i-1];
        data->add(lammpsController->system->simulationTime(), value);
    }
    setInteractive(true);
    return true;
}

bool CPCompute::copyData(ComputeGyration *compute, LAMMPSController *lammpsController) {
    if(!compute) return false;

    // First compute scalar Rg
    double value = compute->scalar;
    setHasScalarData(true);
    setScalarValue(value);
    Data1D *data = ensureExists(QString("Radius of gyration"), true);
    setXLabel("Time");
    setYLabel("Pressure");
    data->add(lammpsController->system->simulationTime(), value);

    // compute->compute_vector();

    // &lt; because < is used as a html tag. Using HTML names instead: http://www.ascii.cl/htmlcodes.htm
    QStringList components = {"xx", "yy", "zz", "xy", "xz", "yz"};

    int numVectorValues = 6;
    for(int i=1; i<=numVectorValues; i++) {
        QString key = components[i-1];
        Data1D *data = ensureExists(key, false);
        double value = compute->vector[i-1];
        data->add(lammpsController->system->simulationTime(), value);
    }
    setInteractive(true);
    return true;
}

void CPCompute::computeInLAMMPS(LAMMPSController *lammpsController) {
    Compute *compute = lammpsController->findComputeByIdentifier(identifier());
    if(compute->scalar_flag == 1) {
        try {
            compute->compute_scalar();
        } catch (LAMMPSException &exception) {
            // TODO: handle this better than just ignoring exception.

//            qDebug() << "ERROR: LAMMPS threw an exception!";
//            qDebug() << "ERROR: File:" << QString::fromStdString(exception.file());
//            qDebug() << "ERROR: Message:" << QString::fromStdString(exception.error());
        }
    }

    if(compute->vector_flag == 1) {
        try {
            compute->compute_vector();
        } catch (LAMMPSException &exception) {
            // TODO: handle this better than just ignoring exception.
//            qDebug() << "ERROR: LAMMPS threw an exception!";
//            qDebug() << "ERROR: File:" << QString::fromStdString(exception.file());
//            qDebug() << "ERROR: Message:" << QString::fromStdString(exception.error());
        }
    }

    if(compute->array_flag == 1) {
        try {
            compute->compute_array();
        } catch (LAMMPSException &exception) {
            // TODO: handle this better than just ignoring exception.
//            qDebug() << "ERROR: LAMMPS threw an exception!";
//            qDebug() << "ERROR: File:" << QString::fromStdString(exception.file());
//            qDebug() << "ERROR: Message:" << QString::fromStdString(exception.error());
        }
    }

    if(compute->peratom_flag == 1) {
        try {
            compute->compute_peratom();
        }  catch(LAMMPSAbortException & ae) {
            qDebug() << "Yeah didn't go so well: " << ae.message.c_str();
        } catch(LAMMPSException & e) { \
            qDebug() << "Yeah didn't go so well: " << e.message.c_str();
        }
    }
}

void CPCompute::copyData(LAMMPSController *lammpsController)
{
    if(m_lastUpdate != -1 && (lammpsController->system->currentTimestep()-m_lastUpdate) < m_frequency) return;

    Compute *lmp_compute = lammpsController->findComputeByIdentifier(identifier());
    if(lmp_compute == nullptr) return; // Didn't find it...

    m_groupBit = lmp_compute->groupbit;
    try {
        if(copyData(lmp_compute, lammpsController)) return;
        if(copyData(dynamic_cast<ComputePressure*>(lmp_compute), lammpsController)) return;
        if(copyData(dynamic_cast<ComputeTemp*>(lmp_compute), lammpsController)) return;
        if(copyData(dynamic_cast<ComputeKE*>(lmp_compute), lammpsController)) return;
        if(copyData(dynamic_cast<ComputePE*>(lmp_compute), lammpsController)) return;
        if(copyData(dynamic_cast<ComputeRDF*>(lmp_compute), lammpsController)) return;
        if(copyData(dynamic_cast<ComputeMSD*>(lmp_compute), lammpsController)) return;
        if(copyData(dynamic_cast<ComputeVACF*>(lmp_compute), lammpsController)) return;
        if(copyData(dynamic_cast<ComputeCOM*>(lmp_compute), lammpsController)) return;
        if(copyData(dynamic_cast<ComputeGyration*>(lmp_compute), lammpsController)) return;
        if(copyData(dynamic_cast<ComputePropertyAtom*>(lmp_compute), lammpsController)) return;
        if(copyData(dynamic_cast<ComputeKEAtom*>(lmp_compute), lammpsController)) return;
        if(copyData(dynamic_cast<ComputeClusterAtom*>(lmp_compute), lammpsController)) return;
        if(copyData(dynamic_cast<ComputeCNAAtom*>(lmp_compute), lammpsController)) return;

    } catch (LAMMPSException &exception) {
        qDebug() << "ERROR: LAMMPS threw an exception!";
        qDebug() << "ERROR: Message:" << QString::fromStdString(exception.message);
        return;
    }
    if(lmp_compute->scalar_flag == 1) {
        try {
            double value = lmp_compute->compute_scalar();
            setHasScalarData(true);
            setScalarValue(value);
            Data1D *data = ensureExists("scalar", true);
            data->add(lammpsController->system->simulationTime(), value, true);
            setInteractive(true);
        } catch (LAMMPSException &exception) {
            qDebug() << "ERROR: LAMMPS threw an exception!";
            qDebug() << "ERROR: Message:" << QString::fromStdString(exception.message);
        }
    }
    if(lmp_compute->vector_flag == 1) {
        try {
            lmp_compute->compute_vector();
            int numVectorValues = lmp_compute->size_vector;
            for(int i=1; i<=numVectorValues; i++) {
                QString key = QString("%1").arg(i);
                Data1D *data = ensureExists(key, true);
                double value = lmp_compute->vector[i-1];
                data->add(lammpsController->system->simulationTime(), value, true);
            }
        } catch (LAMMPSException &exception) {
            qDebug() << "ERROR: LAMMPS threw an exception!";
            qDebug() << "ERROR: Message:" << QString::fromStdString(exception.message);
        }
    }
}

void CPCompute::updateCommand()
{
    // For standard computes, command doesn't change
}

QList<QString> CPCompute::enabledCommands()
{
    return { fullCommand() };
}

QList<QString> CPCompute::disableCommands()
{
    return {QString("uncompute %1").arg(identifier())};
}

bool CPCompute::existsInLammps(LAMMPSController *lammpsController)
{
    Compute *compute = lammpsController->findComputeByIdentifier(identifier());
    return compute!=nullptr;
}

bool CPCompute::isVector() const
{
    return m_isVector;
}

QString CPCompute::group() const
{
    return m_group;
}

int CPCompute::frequency() const
{
    return m_frequency;
}

bool CPCompute::hasScalarData() const
{
    return m_hasScalarData;
}

float CPCompute::scalarValue() const
{
    return m_scalarValue;
}

int CPCompute::num1DData() const
{
    return m_num1DData;
}

QVariantMap CPCompute::data1D() const
{
    return m_data1D;
}

QString CPCompute::xLabel() const
{
    return m_xLabel;
}

QString CPCompute::yLabel() const
{
    return m_yLabel;
}

bool CPCompute::interactive() const
{
    return m_interactive;
}

bool CPCompute::isPerAtom() const
{
    return m_isPerAtom;
}

void CPCompute::setIsVector(bool isVector)
{
    if (m_isVector == isVector)
        return;

    m_isVector = isVector;
    emit isVectorChanged(isVector);
}

void CPCompute::setGroup(QString group)
{
    if (m_group == group)
        return;

    m_group = group;
    emit groupChanged(group);
}

void CPCompute::setFrequency(int frequency)
{
    if (m_frequency == frequency)
        return;

    m_frequency = frequency;
    emit frequencyChanged(frequency);
}

void CPCompute::setHasScalarData(bool hasScalarData)
{
    if (m_hasScalarData == hasScalarData)
        return;

    m_hasScalarData = hasScalarData;
    emit hasScalarDataChanged(hasScalarData);
}

void CPCompute::setScalarValue(float scalarValue)
{
    if (m_scalarValue == scalarValue)
        return;

    m_scalarValue = scalarValue;
    emit scalarValueChanged(scalarValue);
}

void CPCompute::setNum1DData(int num1DData)
{
    if (m_num1DData == num1DData)
        return;

    m_num1DData = num1DData;
    emit num1DDataChanged(num1DData);
}

void CPCompute::setData1D(QVariantMap data1D)
{
    if (m_data1D == data1D)
        return;

    m_data1D = data1D;
    emit data1DChanged(data1D);
}

void CPCompute::setXLabel(QString xLabel)
{
    if (m_xLabel == xLabel)
        return;

    m_xLabel = xLabel;
    emit xLabelChanged(xLabel);
}

void CPCompute::setYLabel(QString yLabel)
{
    if (m_yLabel == yLabel)
        return;

    m_yLabel = yLabel;
    emit yLabelChanged(yLabel);
}

void CPCompute::setInteractive(bool interactive)
{
    if (m_interactive == interactive)
        return;

    m_interactive = interactive;
    emit interactiveChanged(interactive);
}

void CPCompute::setIsPerAtom(bool isPerAtom)
{
    if (m_isPerAtom == isPerAtom)
        return;

    m_isPerAtom = isPerAtom;
    emit isPerAtomChanged(isPerAtom);
}

void CPCompute::setHovered(bool hovered)
{
    if (m_hovered == hovered)
        return;

    m_hovered = hovered;
    emit hoveredChanged(hovered);
}

void CPCompute::setPerAtomIndex(int perAtomIndex)
{
    if (m_perAtomIndex == perAtomIndex)
        return;

    m_perAtomIndex = perAtomIndex;
    emit perAtomIndexChanged(perAtomIndex);
}

void CPCompute::setNumPerAtomValues(int numPerAtomValues)
{
    if (m_numPerAtomValues == numPerAtomValues)
        return;

    m_numPerAtomValues = numPerAtomValues;
    emit numPerAtomValuesChanged(numPerAtomValues);
}

QList<QString> CPCompute::resetCommands()
{
    return { QString("uncompute %1").arg(identifier()), fullCommand() };
}

QString CPCompute::createCommandPrefix()
{
    return QString("compute %1 %2 ").arg(identifier()).arg(group());
}
