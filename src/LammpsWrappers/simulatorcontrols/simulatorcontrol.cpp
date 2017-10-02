#include "simulatorcontrol.h"
#include "lammpscontroller.h"
#include "mysimulator.h"
SimulatorControl::SimulatorControl(QObject *parent) : QObject(parent),
    m_qmlFileName("SimulatorControlItems/SimulatorControlItem.qml"),
    m_groupBit(1) // 1 is group all
{
}

int SimulatorControl::getNextId()
{
    static int nextId = 0;
    return nextId++;
}

SimulatorControl::~SimulatorControl()
{
    emit willBeDestroyed();
}

bool SimulatorControl::hasScalarData() const
{
    return m_hasScalarData;
}

float SimulatorControl::scalarValue() const
{
    return m_scalarValue;
}

QString SimulatorControl::group() const
{
    return m_group;
}

bool SimulatorControl::isPerAtom() const
{
    return m_isPerAtom;
}

int SimulatorControl::perAtomIndex() const
{
    return m_perAtomIndex;
}

int SimulatorControl::numPerAtomValues() const
{
    return m_numPerAtomValues;
}

int SimulatorControl::frequency() const
{
    return m_frequency;
}

QString SimulatorControl::xLabel() const
{
    return m_xLabel;
}

QString SimulatorControl::yLabel() const
{
    return m_yLabel;
}

bool SimulatorControl::interactive() const
{
    return m_interactive;
}

bool SimulatorControl::hovered() const
{
    return m_hovered;
}

QString SimulatorControl::identifier() const
{
    return m_identifier;
}

void SimulatorControl::setIdentifier(QString identifier)
{
    if (m_identifier == identifier)
        return;

    m_identifier = identifier;
    emit identifierChanged(identifier);
}

void SimulatorControl::setData1D(QVariantMap data1D)
{
    if (m_data1D == data1D)
        return;

    m_data1D = data1D;
    emit data1DChanged(data1D);
}

void SimulatorControl::setHasScalarData(bool hasScalarData)
{
    if (m_hasScalarData == hasScalarData)
        return;

    m_hasScalarData = hasScalarData;
    emit hasScalarDataChanged(m_hasScalarData);
}

void SimulatorControl::setScalarValue(float scalarValue)
{
    if (qFuzzyCompare(m_scalarValue, scalarValue))
        return;

    m_scalarValue = scalarValue;
    emit scalarValueChanged(m_scalarValue);
}

void SimulatorControl::setGroup(QString group)
{
    if (m_group == group)
        return;

    m_group = group;
    emit groupChanged(m_group);
}

void SimulatorControl::setIsPerAtom(bool isPerAtom)
{
    if (m_isPerAtom == isPerAtom)
        return;

    m_isPerAtom = isPerAtom;
    emit isPerAtomChanged(m_isPerAtom);
}

void SimulatorControl::setPerAtomIndex(int perAtomIndex)
{
    if (m_perAtomIndex == perAtomIndex)
        return;

    m_perAtomIndex = perAtomIndex;
    emit perAtomIndexChanged(m_perAtomIndex);
}

void SimulatorControl::setNumPerAtomValues(int numPerAtomValues)
{
    if (m_numPerAtomValues == numPerAtomValues)
        return;

    m_numPerAtomValues = numPerAtomValues;
    emit numPerAtomValuesChanged(m_numPerAtomValues);
}

void SimulatorControl::setFrequency(int frequency)
{
    if (m_frequency == frequency)
        return;

    m_frequency = frequency;
    emit frequencyChanged(m_frequency);
}

void SimulatorControl::setXLabel(QString xLabel)
{
    if (m_xLabel == xLabel)
        return;

    m_xLabel = xLabel;
    emit xLabelChanged(m_xLabel);
}

void SimulatorControl::setYLabel(QString yLabel)
{
    if (m_yLabel == yLabel)
        return;

    m_yLabel = yLabel;
    emit yLabelChanged(m_yLabel);
}

void SimulatorControl::setInteractive(bool interactive)
{
    if (m_interactive == interactive)
        return;

    m_interactive = interactive;
    emit interactiveChanged(m_interactive);
}

void SimulatorControl::setHovered(bool hovered)
{
    if (m_hovered == hovered)
        return;

    m_hovered = hovered;
    emit hoveredChanged(m_hovered);
}

void SimulatorControl::setType(QString type)
{
    if (m_type == type)
        return;

    m_type = type;
    emit typeChanged(m_type);
}

void SimulatorControl::setQmlFileName(QUrl qmlFileName)
{
    if (m_qmlFileName == qmlFileName)
        return;

    m_qmlFileName = qmlFileName;
    emit qmlFileNameChanged(m_qmlFileName);
}

void SimulatorControl::setWindow(QQuickWindow *window)
{
    if (m_window == window)
        return;
    m_window = window;
    emit windowChanged(m_window);
}

Data1D *SimulatorControl::ensureExists(QString key, bool enabledByDefault) {
    if(!m_data1DRaw.contains(key)) {
        Data1D *data = new Data1D();
        data->setEnabled(enabledByDefault);
        m_data1DRaw.insert(key, data);
        m_data1D.insert(key, QVariant::fromValue<Data1D*>(data));
    }
    return m_data1DRaw[key];
}

const std::vector<double> &SimulatorControl::atomData() const
{
    return m_atomData;
}

QString SimulatorControl::type() const
{
    return m_type;
}

QVariantMap SimulatorControl::data1D() const
{
    return m_data1D;
}

QUrl SimulatorControl::qmlFileName() const
{
    return m_qmlFileName;
}

QQuickWindow *SimulatorControl::window() const
{
    return m_window;
}

int SimulatorControl::groupBit() const
{
    return m_groupBit;
}

void SimulatorControl::exportToTextFile(QString fileName)
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

void SimulatorControl::exportToPythonFile(QString fileName)
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
        const QVector<QPointF> &points = m_data1DRaw[key]->points();

        if(key == keys[0]) {
            qDebug() << "X label: " << xLabel().toLower();
            out << xLabel().toLower() << " = np.asarray([";
            for(auto iter = points.begin(); iter != points.end(); iter++) {
                if (iter != points.begin()) out << ", ";
                out << (*iter).x();
            }
            out << "])\n";
        }

        out << key.toLower().replace(" ", "_") << " = np.asarray([";
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
        out << "plt.plot(" << xLabel().toLower() << ", " << key.toLower().replace(" ", "_") << ", label=u'" << label << "')\n";
    }
    out << "plt.xlabel('" << xLabel() << "')\n"; // TODO: add units
    out << "plt.ylabel('" << yLabel() << "')\n";
    out << "plt.legend()\n";
    out << "plt.show()\n";

    file.close();
}

void SimulatorControl::exportToMatlabFile(QString fileName)
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
        const QVector<QPointF> &points = m_data1DRaw[key]->points();

        if(key == keys[0]) {
            out << xLabel().toLower() << " = [";
            for(auto iter = points.begin(); iter != points.end(); iter++) {
                if (iter != points.begin()) out << ", ";
                out << (*iter).x();
            }
            out << "];\n";
        }

        out << key.toLower().replace(" ", "_") << " = [";
        for(auto iter = points.begin(); iter != points.end(); iter++) {
            if (iter != points.begin()) out << ", ";
            out << (*iter).y();
        }
        out << "];\n";
    }
    out << "\n";

    for(QString key : keys) {
        out << "plot(" << xLabel().toLower() << ", " << key.toLower().replace(" ", "_") << ", 'LineWidth',2);\n";
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

void SimulatorControl::clear()
{
    for(Data1D *data : m_data1DRaw) {
        data->clear();
        emit data->updated();
    }
}

void SimulatorControl::updateData1D()
{
    for(Data1D *data : m_data1DRaw) {
        emit data->updated();
    }
}
