#include "cpcompute.h"
#include "lammpscontroller.h"
#include "mysimulator.h"
#include "../system.h"
#include <QDebug>
#include <error.h>
#include <update.h>
#include <atom.h>

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

void CPCompute::exportToTextFile(QString fileName)
{
    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        file.setFileName(file.fileName().replace("file://",""));
        if(!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            qDebug() << "Error, could not open file " << fileName;  // TODO: proper error handling
            return;
        }
    }

    QTextStream out(&file);

    QStringList keys = m_data1DRaw.keys();

    // Print header with column names
    out << "# " << xLabel() << " ";
    for(QString key : keys) {
        out << key << " ";
    }
    out << "\n";

    int dataCount = m_data1DRaw[keys[0]]->points().size();
    for(int i=0; i<dataCount; i++) {
        float x = m_data1DRaw[keys[0]]->points()[i].x();
        out << x << " ";

        for(QString key : keys) {
            float y = m_data1DRaw[key]->points()[i].y();
            out << y << " ";
        }
        out << "\n";
    }
    file.close();
}

void CPCompute::exportToPythonFile(QString fileName)
{
    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        file.setFileName(file.fileName().replace("file://",""));
        if(!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            qDebug() << "Error, could not open file " << fileName;  // TODO: proper error handling
            return;
        }
    }

    QTextStream out(&file);
    out << "# -*- coding: utf-8 -*-\n";
    out << "import matplotlib.pyplot as plt\n";
    out << "import numpy as np\n";

    QStringList keys = m_data1DRaw.keys();
    // Print arrays to variables
    for(QString key : keys) {
        const QList<QPointF> &points = m_data1DRaw[key]->points();

        if(key == keys[0]) {
            qDebug() << "X label: " << xLabel().toLower();
            out << xLabel().toLower() << " = np.asarray([";
            for(auto iter = points.begin(); iter != points.end(); iter++) {
                if (iter != points.begin()) out << ", ";
                out << (*iter).x();
            }
            out << "])\n";
        }

        out << key.toLower() << " = np.asarray([";
        for(auto iter = points.begin(); iter != points.end(); iter++) {
            if (iter != points.begin()) out << ", ";
            out << (*iter).y();
        }
        out << "])\n";
    }
    out << "\n";

    for(QString key : keys) {
        QString label = key;
        if(!m_data1DRaw[key]->label().isEmpty()) {
            label = m_data1DRaw[key]->label();
            label.replace("<sup>", "^");
            label.replace("</sup>", "");
        }
        out << "plt.plot(" << xLabel().toLower() << ", " << key.toLower() << ", label=u'" << label << "')\n";
    }
    out << "plt.xlabel('" << xLabel() << "')\n"; // TODO: add units
    out << "plt.ylabel('" << yLabel() << "')\n";
    out << "plt.legend()\n";
    out << "plt.show()\n";

    file.close();
}

void CPCompute::exportToMatlabFile(QString fileName)
{
    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        file.setFileName(file.fileName().replace("file://",""));
        if(!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            qDebug() << "Error, could not open file " << fileName;  // TODO: proper error handling
            return;
        }
    }

    QTextStream out(&file);

    QStringList keys = m_data1DRaw.keys();
    // Print arrays to variables
    for(QString key : keys) {
        const QList<QPointF> &points = m_data1DRaw[key]->points();

        if(key == keys[0]) {
            out << xLabel().toLower() << " = [";
            for(auto iter = points.begin(); iter != points.end(); iter++) {
                if (iter != points.begin()) out << ", ";
                out << (*iter).x();
            }
            out << "];\n";
        }

        out << key.toLower() << " = [";
        for(auto iter = points.begin(); iter != points.end(); iter++) {
            if (iter != points.begin()) out << ", ";
            out << (*iter).y();
        }
        out << "];\n";
    }
    out << "\n";

    for(QString key : keys) {
        out << "plot(" << xLabel().toLower() << ", " << key.toLower() << ", 'LineWidth',2);\n";
        if(key==keys.first()) {
            out << "hold('on');\n";
        }
    }
    out << "set(gca,'fontsize', 16);\n";
    out << "xlabel('" << xLabel() << "');\n"; // TODO: add units
    out << "ylabel('" << yLabel() << "');\n";
    out << "legend(";
    for(auto iter = keys.begin(); iter != keys.end(); iter++) {
        if (iter != keys.begin()) out << ", ";
        QString key = *iter;
        QString label = key;
        if(!m_data1DRaw[key]->label().isEmpty()) {
            label = m_data1DRaw[key]->label();
            label.replace("<sup>", "^");
            label.replace("</sup>", "");
            label.replace("∆", "\\Delta ");
        }

        out << "'" << label << "'";
    }
    out << ");\n";

    file.close();
}

bool CPCompute::copyData(Compute *compute, LAMMPSController *lammpsController) {
    // Handles per atom computes
    if(!compute) return false;
    if(!compute->peratom_flag) return false;
    // if(compute->size_peratom_cols > 0) return false;
    int numAtoms = lammpsController->system->numberOfAtoms();
    int numCols = compute->size_peratom_cols;

    if(numCols == 0) {
        setNumPerAtomValues(1);
        double *values = compute->vector_atom;
        m_atomData = std::vector<double>(values, values+numAtoms);
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

bool CPCompute::copyData(ComputeTemp *compute, LAMMPSController *lammpsController) {
    if(!compute) return false;
    double value = compute->scalar;
    setHasScalarData(true);
    setScalarValue(value);
    Data1D *data = ensureExists(QString("Temperature"), true);
    setXLabel("Time");
    setYLabel("Temperature");
    data->add(lammpsController->system->simulationTime(), value, true);
    setInteractive(true);
    return true;
}

bool CPCompute::copyData(ComputePE *compute, LAMMPSController *lammpsController) {
    if(!compute) return false;
    if(lammpsController->lammps()->update->ntimestep != lammpsController->lammps()->update->eflag_global) return true;
    double value = compute->scalar;
    setHasScalarData(true);
    setScalarValue(value);
    Data1D *data = ensureExists(QString("PotentialEnergy"), true);
    data->setLabel("Potential energy");
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
    Data1D *data = ensureExists(QString("KineticEnergy"), true);
    data->setLabel("Kinetic energy");
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
    QStringList components = {"Pxx", "Pyy", "Pzz", "Pxy", "Pxz", "Pyz"};

    int numVectorValues = 6;
    for(int i=0; i<numVectorValues; i++) {
        QString key = components[i];
        Data1D *data = ensureExists(key, false);
        data->setLabel(components[i]);
        double value = compute->vector[i];
        data->add(lammpsController->system->simulationTime(), value);
    }
    setInteractive(true);
    return true;
}

bool CPCompute::copyData(ComputeRDF *compute, LAMMPSController *lammpsController) {
    Q_UNUSED(lammpsController);
    if(!compute) return false;
    int numBins = compute->size_array_rows;         // rows in global array
    int numColumns = compute->size_array_cols;      // columns in global array
    int numPairs = (numColumns - 1)/2;

    for(int pairId=0; pairId<numPairs; pairId++) {
        QString key = QString("Pair_%1").arg(pairId+1);
        QString label = QString("Pair %1").arg(pairId+1);
        Data1D *data = ensureExists(key, true);
        data->setLabel(label);
        data->clear(true);

        for(int bin=0; bin<numBins; bin++) {
            double r = compute->array[bin][0];
            double rdf = compute->array[bin][1+2*pairId];
            data->add(r,rdf,true);
        }
    }
    setXLabel("r");
    setYLabel("RDF");
    setInteractive(true);
    return true;
}

bool CPCompute::copyData(ComputeMSD *compute, LAMMPSController *lammpsController) {
    if(!compute) return false;
    // compute->compute_vector();

    // http://www.ascii.cl/htmlcodes.htm
    // QStringList components = {"∆x<sup>2</sup>", "∆y<sup>2</sup>", "∆z<sup>2</sup>", "∆r<sup>2</sup>"};
    QStringList components = {"dx2", "dy2", "dz2", "dr2"};
    QStringList labels = {"∆x<sup>2</sup>", "∆y<sup>2</sup>", "∆z<sup>2</sup>", "∆r<sup>2</sup>"};

    int numVectorValues = 4;
    for(int i=0; i<numVectorValues; i++) {
        QString key = components[i];
        Data1D *data = ensureExists(key, false);
        data->setLabel(labels[i]);
        double value = compute->vector[i];
        data->add(lammpsController->system->simulationTime(), value);
    }
    setXLabel("Time");
    setYLabel("Mean square displacement");
    setInteractive(true);
    return true;
}

bool CPCompute::copyData(ComputeVACF *compute, LAMMPSController *lammpsController) {
    if(!compute) return false;
    // compute->compute_vector();

    // &lt; because < is used as a html tag. Using HTML names instead: http://www.ascii.cl/htmlcodes.htm
    QStringList components = {"vx2", "vy2", "vz2", "vr2"};
    QStringList labels = {"&lt;vx, vx0\&gt;", "&lt;vy, vy0&gt;", "&lt;vz, vz0&gt;", "&lt;v, v0&gt;"};
    int numVectorValues = 4;
    for(int i=0; i<numVectorValues; i++) {
        QString key = components[i];
        Data1D *data = ensureExists(key, false);
        data->setLabel(labels[i]);
        double value = compute->vector[i];
        data->add(lammpsController->system->simulationTime(), value);
    }
    setXLabel("Time");
    setYLabel("VACF");
    setInteractive(true);
    return true;
}

bool CPCompute::copyData(ComputeCOM *compute, LAMMPSController *lammpsController) {
    if(!compute) return false;
    // compute->compute_vector();

    // &lt; because < is used as a html tag. Using HTML names instead: http://www.ascii.cl/htmlcodes.htm
    QStringList components = {"x_cm", "y_cm", "z_cm"};

    int numVectorValues = 3;
    for(int i=0; i<numVectorValues; i++) {
        QString key = components[i];
        Data1D *data = ensureExists(key, false);
        data->setLabel(key);
        double value = compute->vector[i];
        data->add(lammpsController->system->simulationTime(), value);
    }
    setXLabel("Time");
    setYLabel("COM");
    setInteractive(true);
    return true;
}

bool CPCompute::copyData(ComputeGyration *compute, LAMMPSController *lammpsController) {
    if(!compute) return false;

    // First compute scalar Rg
    double value = compute->scalar;
    setHasScalarData(true);
    setScalarValue(value);
    Data1D *data = ensureExists(QString("RadiusOfGyration"), true);
    data->setLabel("Rg");
    setXLabel("Time");
    setYLabel("Radius of gyration");
    data->add(lammpsController->system->simulationTime(), value);

    // compute->compute_vector();

    // &lt; because < is used as a html tag. Using HTML names instead: http://www.ascii.cl/htmlcodes.htm
    QStringList components = {"Rxx", "Ryy", "Rzz", "Rxy", "Rxz", "Rcyz"};

    int numVectorValues = 6;
    for(int i=0; i<numVectorValues; i++) {
        QString key = components[i];
        Data1D *data = ensureExists(key, false);
        data->setLabel(components[i]);
        double value = compute->vector[i];
        data->add(lammpsController->system->simulationTime(), value);
    }
    setXLabel("Time");
    setYLabel("Rg");
    setInteractive(true);
    return true;
}

bool CPCompute::validateStatus(Compute *compute, LAMMPS *lammps) {
    if( (compute->peflag||compute->peatomflag) && lammps->update->ntimestep != lammps->update->eflag_global) return false;
    if( (compute->pressflag||compute->pressatomflag) && lammps->update->ntimestep != lammps->update->vflag_global) return false;
    return true;
}

void CPCompute::computeInLAMMPS(LAMMPSController *lammpsController) {
    Compute *compute = lammpsController->findComputeByIdentifier(identifier());
    if(compute->scalar_flag == 1) {
        if(validateStatus(compute, lammpsController->lammps())) {
            try {
                compute->compute_scalar();
            } catch (LAMMPSException &exception) {
                // TODO: handle this better than just ignoring exception.
            }
        }
    }

    if(compute->vector_flag == 1) {
        if(validateStatus(compute, lammpsController->lammps())) {
            try {
                compute->compute_vector();
            } catch (LAMMPSException &exception) {
                // TODO: handle this better than just ignoring exception.
            }
        }
    }

    if(compute->array_flag == 1) {
        if(validateStatus(compute, lammpsController->lammps())) {
            try {
                compute->compute_array();
            } catch (LAMMPSException &exception) {
                // TODO: handle this better than just ignoring exception.
            }
        }
    }

    if(compute->peratom_flag == 1) {
        if(validateStatus(compute, lammpsController->lammps())) {
            try {
                compute->compute_peratom();
            }  catch(LAMMPSAbortException & ae) {
                qDebug() << "Yeah didn't go so well: " << ae.message.c_str();
            } catch(LAMMPSException & e) { \
                qDebug() << "Yeah didn't go so well: " << e.message.c_str();
            }
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
            setXLabel("Time");
            setYLabel("Value");
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
                QString key = QString("value_%1").arg(i);
                Data1D *data = ensureExists(key, true);
                double value = lmp_compute->vector[i-1];
                data->add(lammpsController->system->simulationTime(), value, true);
            }
            setXLabel("Time");
            setYLabel("Value");
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

QList<QString> CPCompute::resetCommands()
{
    return { QString("uncompute %1").arg(identifier()), fullCommand() };
}

QString CPCompute::createCommandPrefix()
{
    return QString("compute %1 %2 ").arg(identifier()).arg(group());
}
