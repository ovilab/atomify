#include "cpcompute.h"
#include "system.h"
#include "lammpscontroller.h"
#include "mysimulator.h"
#include "data1d.h"
#include <QDebug>
CPCompute::CPCompute(QQuickItem *parent) : SimulatorControl(parent)
{
    m_scalarValue = new Data1D(this);
}

CPCompute::~CPCompute() { }

void CPCompute::updateCommand()
{
    // For standard computes, command doesn't change
}

void CPCompute::update(LAMMPSController *lammpsController)
{
    if(lammpsController->system()->timesteps() == m_lastUpdated) return;
    QElapsedTimer t;
    t.start();
    SimulatorControl::update(lammpsController);
    LAMMPS_NS::Compute *lmp_compute = lammpsController->findComputeByIdentifier(identifier());
    if(lmp_compute != nullptr) {
        double time = lammpsController->system()->simulationTime();
        if(lmp_compute->scalar_flag) {
            double val = lmp_compute->compute_scalar();
            m_scalarValue->add(time, val);
            setValue(val);
        }
        if(lmp_compute->vector_flag) {
            lmp_compute->compute_vector();
            double *values = lmp_compute->vector;
            int numValues = lmp_compute->size_vector;
            m_values.clear();
           for(int i=0; i<numValues; i++) {
               m_values.push_back(values[i]);
               if(i < m_vectorTitles.size()) {
                   Data1D *data = m_vectorValuesRaw[m_vectorTitles[i]];
                   data->add(time, values[i]);
               }
           }
           emit valuesChanged(m_values);
        }
    }
    // qDebug() << "Compute " << identifier() << " spent " << t.elapsed() << " ms to update.";
}

QList<QString> CPCompute::enabledCommands()
{
    return { QString("compute %1 %2").arg(identifier()).arg(command()) };
}

QList<QString> CPCompute::disableCommands()
{
    return {QString("uncompute %1").arg(identifier())};
}

bool CPCompute::existsInLammps(LAMMPSController *lammpsController)
{
    LAMMPS_NS::Compute *compute = lammpsController->findComputeByIdentifier(identifier());
    return compute!=nullptr;
}

float CPCompute::value() const
{
    return m_value;
}

QList<qreal> CPCompute::values() const
{
    return m_values;
}

QStringList CPCompute::vectorTitles() const
{
    return m_vectorTitles;
}

QString CPCompute::scalarTitle() const
{
    return m_scalarTitle;
}

Data1D *CPCompute::scalarValue() const
{
    return m_scalarValue;
}

QVariantMap CPCompute::vectorValues() const
{
    return m_vectorValues;
}

void CPCompute::setValue(float value)
{
    if (m_value == value)
        return;

    m_value = value;
    emit valueChanged(value);
}

void CPCompute::setValues(QList<qreal> values)
{
    if (m_values == values)
        return;

    m_values = values;
    emit valuesChanged(values);
}

void CPCompute::setVectorTitles(QStringList vectorTitles)
{
    if (m_vectorTitles == vectorTitles)
        return;

    m_vectorTitles = vectorTitles;
    for(QString key : vectorTitles) {
        ensureExists(key, true);
    }

    emit vectorTitlesChanged(vectorTitles);
}

void CPCompute::setScalarTitle(QString scalarTitle)
{
    if (m_scalarTitle == scalarTitle)
        return;

    m_scalarTitle = scalarTitle;
    m_scalarValue->setTitle(scalarTitle);
    emit scalarTitleChanged(scalarTitle);
}

void CPCompute::setScalarValue(Data1D *scalarValue)
{
    if (m_scalarValue == scalarValue)
        return;

    m_scalarValue = scalarValue;
    emit scalarValueChanged(scalarValue);
}

void CPCompute::setVectorValues(QVariantMap vectorValues)
{
    if (m_vectorValues == vectorValues)
        return;

    m_vectorValues = vectorValues;
    emit vectorValuesChanged(vectorValues);
}

Data1D *CPCompute::ensureExists(QString key, bool enabledByDefault) {
    if(!m_vectorValuesRaw.contains(key)) {
        Data1D *data = new Data1D(this);
        data->setTitle(key);
        data->setEnabled(enabledByDefault);
        m_vectorValuesRaw.insert(key, data);
        m_vectorValues.insert(key, QVariant::fromValue<Data1D*>(data));
    }
    return m_vectorValuesRaw[key];
}

QList<QString> CPCompute::resetCommands()
{
    return { QString("uncompute %1").arg(identifier()), QString("compute %1 %2").arg(identifier()).arg(command()) };
}
