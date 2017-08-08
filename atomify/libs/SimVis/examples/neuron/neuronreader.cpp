#include "neuronreader.h"

#include <QDebug>
#include <QFile>
#include <QXmlStreamReader>
#include <QQmlFile>

NeuronWorker::NeuronWorker()
{
    reset();
}

void NeuronWorker::synchronizeSimulator(Simulator *simulator)
{
    Q_UNUSED(simulator)
}

void NeuronWorker::work()
{
}

void NeuronWorker::reset()
{
    m_segments.clear();
}


NeuronReader::NeuronReader(QNode *parent)
    : Simulator(parent)
    , m_cylinderData(new CylinderData(this))
{

}

NeuronReader::~NeuronReader()
{

}

QUrl NeuronReader::source() const
{
    return m_source;
}

void NeuronReader::readFile()
{
    QString fileNameString = QQmlFile::urlToLocalFileOrQrc(m_source);
    QFile file(fileNameString);
    file.open(QFile::ReadOnly);
    if(!file.isOpen()) {
        qWarning() << "Could not read file" << fileNameString;
        return;
    }
    QXmlStreamReader reader(&file);
    //    QList<Student> students;
    reader.readNext();
    //Reading from the file
    Segment segment;
    while (!reader.isEndDocument())
    {
        if(reader.isStartElement()) {
            //            qDebug() << reader.name() << reader.isStartElement() << reader.isEndElement();
            if(reader.name() == "segment") {
                m_segments.append(segment);
                segment = Segment();
                segment.id = reader.attributes().value("id").toInt();
                if(reader.attributes().hasAttribute("parent")) {
                    segment.parentID = reader.attributes().value("parent").toInt();
                    segment.hasParentID = true;
                }
            }
            if(reader.name() == "parent") {
                segment.parentID = reader.attributes().value("segment").toInt();
                segment.hasParentID = true;
            }
            if(reader.name() == "proximal") {
                //                qDebug() << reader.attributes().value("x") << reader.attributes().value("y") << reader.attributes().value("z");
                segment.proximal.setX(reader.attributes().value("x").toDouble());
                segment.proximal.setY(reader.attributes().value("y").toDouble());
                segment.proximal.setZ(reader.attributes().value("z").toDouble());
                segment.proximalWidth = reader.attributes().value("diameter").toDouble();
                segment.hasProximal = true;
            }
            if(reader.name() == "distal") {
                //                qDebug() << reader.attributes().value("x") << reader.attributes().value("y") << reader.attributes().value("z") << reader.attributes().value("diameter");
                segment.distal.setX(reader.attributes().value("x").toDouble());
                segment.distal.setY(reader.attributes().value("y").toDouble());
                segment.distal.setZ(reader.attributes().value("z").toDouble());
                segment.distalWidth = reader.attributes().value("diameter").toDouble();
                segment.hasDistal = true;
            }
        }
        reader.readNext();
    }

    m_segments.append(segment);
    for(Segment &segment : m_segments) {
        if(!segment.hasProximal && segment.hasParentID) {
            for(Segment &otherSegment : m_segments) {
                if(otherSegment.id == segment.parentID) {
                    Segment &parent = otherSegment;
                    segment.proximal = parent.distal;
                    break;
                }
            }
        }
        if(segment.proximalWidth == 0.0) {
            segment.proximalWidth = segment.distalWidth;
        }
    }

    m_cylinders.clear();
    for(int i = 0; i < 1; i++) {
        for(int j = 0; j < 1; j++) {
            for(const Segment &segment : m_segments) {
                CylinderVBOData cylinder;

                float scaler = 0.1;

                cylinder.vertex1 = segment.proximal * scaler + QVector3D(i*4, 0, j*4);
                cylinder.vertex2 = segment.distal * scaler + QVector3D(i*4, 0, j*4);;
                cylinder.radius1 = segment.proximalWidth * scaler * 0.5;
                cylinder.radius2 = segment.distalWidth * scaler * 0.5;

                m_cylinders.push_back(cylinder);
            }
        }
    }

    //    m_cylinders.clear();
    //    CylinderVBOData cylinder;
    //    cylinder.vertex1 = QVector3D(1.0, -2.0,  1.0);
    //    cylinder.vertex2 = QVector3D(-1.0,  2.0,  -1.0);
    //    cylinder.radius1 = 2.4;
    //    cylinder.radius2 = 1.2;

    //    m_cylinders.push_back(cylinder);

    m_cylinderData->setData(m_cylinders);


    qDebug() << "Done reading";
    m_segmentsAreDirty = true;
}

void NeuronReader::setSource(QUrl arg)
{
    if (m_source == arg)
        return;

    m_source = arg;
    readFile();
    emit sourceChanged(arg);
}

SimulatorWorker *NeuronReader::createWorker()
{
    return new NeuronWorker();
}
