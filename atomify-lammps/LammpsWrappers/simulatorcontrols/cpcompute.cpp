#include "cpcompute.h"
#include "lammpscontroller.h"
#include "mysimulator.h"
#include "../system.h"
#include <QDebug>
#include <error.h>
#include <update.h>

CPCompute::CPCompute(Qt3DCore::QNode *parent) : SimulatorControl(parent)
{

}

CPCompute::~CPCompute() { }

CP1DData *CPCompute::ensureExists(QString key, bool enabledByDefault) {
    if(!m_data1DRaw.contains(key)) {
        CP1DData *data = new CP1DData(this);
        data->setEnabled(enabledByDefault);
        m_data1DRaw.insert(key, data);
        m_data1D.insert(key, QVariant::fromValue<CP1DData*>(data));
    }
    return m_data1DRaw[key];
}

bool CPCompute::copyData(ComputeKEAtom *compute, LAMMPSController *lammpsController) {
    if(!compute) return false;
    double *data = compute->vector_atom;
    return true;
}

bool CPCompute::copyData(ComputeTemp *compute, LAMMPSController *lammpsController) {
    if(!compute) return false;
    double value = compute->scalar;
    setHasScalarData(true);
    setScalarValue(value);
    CP1DData *data = ensureExists(QString("Temperature"), true);
    setXLabel("Time");
    setYLabel("Temperature");
    data->add(lammpsController->system()->simulationTime(), value);
    setInteractive(true);
    return true;
}

bool CPCompute::copyData(ComputePE *compute, LAMMPSController *lammpsController) {
    if(!compute) return false;
    if(lammpsController->lammps()->update->ntimestep != lammpsController->lammps()->update->eflag_global) return true;
    double value = compute->scalar;
    setHasScalarData(true);
    setScalarValue(value);
    CP1DData *data = ensureExists(QString("Potential energy"), true);
    setXLabel("Time");
    setYLabel("Potential Energy");
    data->add(lammpsController->system()->simulationTime(), value);
    setInteractive(true);
    return true;
}

bool CPCompute::copyData(ComputeKE *compute, LAMMPSController *lammpsController) {
    if(!compute) return false;
    double value = compute->scalar;
    setHasScalarData(true);
    setScalarValue(value);
    CP1DData *data = ensureExists(QString("Kinetic energy"), true);
    setXLabel("Time");
    setYLabel("Kinetic Energy");
    data->add(lammpsController->system()->simulationTime(), value);
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
    CP1DData *data = ensureExists(QString("Pressure"), true);
    setXLabel("Time");
    setYLabel("Pressure");
    data->add(lammpsController->system()->simulationTime(), value);

    // Then compute stress tensor
    // compute->compute_vector();
    // xx, yy, zz, xy, xz, yz
    QStringList components = {"xx", "yy", "zz", "xy", "xz", "yz"};

    int numVectorValues = 6;
    for(int i=1; i<=numVectorValues; i++) {
        QString key = components[i-1];
        CP1DData *data = ensureExists(key, false);
        double value = compute->vector[i-1];
        data->add(lammpsController->system()->simulationTime(), value);
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
        CP1DData *data = ensureExists(key, true);
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
        CP1DData *data = ensureExists(key, false);
        double value = compute->vector[i-1];
        data->add(lammpsController->system()->simulationTime(), value);
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
        CP1DData *data = ensureExists(key, false);
        double value = compute->vector[i-1];
        data->add(lammpsController->system()->simulationTime(), value);
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
        CP1DData *data = ensureExists(key, false);
        double value = compute->vector[i-1];
        data->add(lammpsController->system()->simulationTime(), value);
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
    CP1DData *data = ensureExists(QString("Radius of gyration"), true);
    setXLabel("Time");
    setYLabel("Pressure");
    data->add(lammpsController->system()->simulationTime(), value);

    // compute->compute_vector();

    // &lt; because < is used as a html tag. Using HTML names instead: http://www.ascii.cl/htmlcodes.htm
    QStringList components = {"xx", "yy", "zz", "xy", "xz", "yz"};

    int numVectorValues = 6;
    for(int i=1; i<=numVectorValues; i++) {
        QString key = components[i-1];
        CP1DData *data = ensureExists(key, false);
        double value = compute->vector[i-1];
        data->add(lammpsController->system()->simulationTime(), value);
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
            compute->compute_scalar();
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
        } catch (LAMMPSException &exception) {
            // TODO: handle this better than just ignoring exception.
//            qDebug() << "ERROR: LAMMPS threw an exception!";
//            qDebug() << "ERROR: File:" << QString::fromStdString(exception.file());
//            qDebug() << "ERROR: Message:" << QString::fromStdString(exception.error());
        }
    }
}

void CPCompute::copyData(LAMMPSController *lammpsController)
{
    // if(lammpsController->system()->timesteps() % m_frequency != 0) return;
    if(lastUpdate != -1 && (lammpsController->system()->timesteps()-lastUpdate) < m_frequency) return;
    // if(lammpsController->system()->timesteps() % m_frequency != 0) return;
    Compute *lmp_compute = lammpsController->findComputeByIdentifier(identifier());
    if(lmp_compute == nullptr) return;
    try {
        if(copyData(dynamic_cast<ComputePressure*>(lmp_compute), lammpsController)) return;
        if(copyData(dynamic_cast<ComputeTemp*>(lmp_compute), lammpsController)) return;
        if(copyData(dynamic_cast<ComputeKE*>(lmp_compute), lammpsController)) return;
        if(copyData(dynamic_cast<ComputePE*>(lmp_compute), lammpsController)) return;
        if(copyData(dynamic_cast<ComputeRDF*>(lmp_compute), lammpsController)) return;
        if(copyData(dynamic_cast<ComputeMSD*>(lmp_compute), lammpsController)) return;
        if(copyData(dynamic_cast<ComputeVACF*>(lmp_compute), lammpsController)) return;
        if(copyData(dynamic_cast<ComputeCOM*>(lmp_compute), lammpsController)) return;
        if(copyData(dynamic_cast<ComputeGyration*>(lmp_compute), lammpsController)) return;
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
            CP1DData *data = ensureExists("scalar", true);
            data->add(lammpsController->system()->simulationTime(), value, true);
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
                CP1DData *data = ensureExists(key, true);
                double value = lmp_compute->vector[i-1];
                data->add(lammpsController->system()->simulationTime(), value, true);
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

int CPCompute::maxCount() const
{
    return m_maxCount;
}

bool CPCompute::interactive() const
{
    return m_interactive;
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

void CPCompute::setMaxCount(int maxCount)
{
    if (m_maxCount == maxCount)
        return;

    m_maxCount = maxCount;
    foreach (CP1DData *data, m_data1DRaw) {
        data->setMaxCount(m_maxCount);
    }
    emit maxCountChanged(maxCount);
}

void CPCompute::setInteractive(bool interactive)
{
    if (m_interactive == interactive)
        return;

    m_interactive = interactive;
    emit interactiveChanged(interactive);
}

QList<QString> CPCompute::resetCommands()
{
    return { QString("uncompute %1").arg(identifier()), fullCommand() };
}

QString CPCompute::createCommandPrefix()
{
    return QString("compute %1 %2 ").arg(identifier()).arg(group());
}
